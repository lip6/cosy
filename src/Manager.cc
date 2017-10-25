/* Copyright 2017 Hakan Metin - All rights reserved */

#include "cosy/Manager.h"
#include "cosy/Printer.h"

using cosy::Manager;
using cosy::Permutation;
using cosy::Lit;
using cosy::Var;


Manager::Manager(unsigned int num_vars) :
    _num_vars(num_vars),
    _assigns(Assigns(num_vars)),
    _opt_propagator_active(false),
    _num_conflicts(0),
    _num_propagations(0) {
    _watchers.assign(num_vars + 1, std::unordered_set<int>());
}

Manager::~Manager() {
}

void Manager::addPermutation(std::unique_ptr<Permutation> permutation) {
    assert(permutation != nullptr);

    const unsigned int permutation_index = _permutations.size();
    const unsigned int num_cycles = permutation->numCycles();
    std::unique_ptr<Symmetry> symmetry;
    Lit element;

    if (permutation->numCycles() == 0)
        return;

    symmetry = std::unique_ptr<Symmetry>(new Symmetry(_assigns,
                                                      permutation_index));

    if (permutation->size() > _watchers.size())
        _watchers.resize(permutation->size() + 1);

    for (unsigned int c=0; c < num_cycles; c++) {
        element = permutation->lastElementInCycle(c);
        for (const Lit image : permutation->cycle(c)) {
            _watchers[varOf(element)].insert(permutation_index);
            symmetry->_inverse[image] = element;
            symmetry->_inverse[negate(image)] = negate(element);

            _symmetric_vars.insert(varOf(image));

            if (negate(element) == image)
                _inverting.insert(varOf(element));

            element = image;
        }
    }

    _permutations.emplace_back(permutation.release());
    _symmetries.emplace_back(symmetry.release());
}

void Manager::augmentGenerators() {
    std::unique_ptr<Permutation> permutation_inverse = nullptr;
    std::vector<int> cycle;
    unsigned int size = _permutations.size();
    unsigned int augment_times;

    Printer::print("Number Generator before augment", _permutations.size());
    for (unsigned int i=0; i<size; i++) {
        Permutation *permutation = _permutations[i].get();

        augment_times = permutation->order() - 2;

        for (unsigned int augment=0; augment < augment_times; augment++) {
            permutation_inverse = std::unique_ptr<Permutation>
                (new Permutation(_num_vars));
            for (int c=0; c<permutation->numCycles(); ++c) {
                cycle.assign(permutation->cycle(c).begin(),
                             permutation->cycle(c).end());
                std::reverse(cycle.begin()+1, cycle.end());

                for (const int& element : cycle)
                    permutation_inverse->addToCurrentCycle(element);
                permutation_inverse->closeCurrentCycle();
            }

            addPermutation(std::move(permutation_inverse));
            permutation = _permutations.back().get();
        }
    }
    Printer::print("Number Generator after  augment", _permutations.size());
}

const std::vector<Permutation*> Manager::permutationsRef() const {
    std::vector<Permutation*> ref;
    for (const std::unique_ptr<Permutation>& p : _permutations)
        ref.push_back(p.get());

    return ref;
}



void Manager::notify(Lit lit, unsigned int level) {
    _assigns.setLitTrue(lit, level);

    if (!_minimality.minimal()) {
        return;
    }

    for (const int perm_index : _watchers[varOf(lit)]) {
        const std::unique_ptr<Symmetry>& symmetry = _symmetries[perm_index];

        if (symmetry->inactive())
            continue;

        symmetry->forwardLookupIndex(lit);
        symmetry->verify(lit, &_minimality, _lex_order);

        if (_opt_propagator_active)
            symmetry->verify(lit, &_propagator, _lex_order);
    }
}

void Manager::cancel(Lit lit) {
    _assigns.setLitUndef(lit);

    for (const int permutation_index : _watchers[varOf(lit)]) {
        std::unique_ptr<Symmetry>& symmetry = _symmetries[permutation_index];

        symmetry->backwardLookupIndex(lit);

        _minimality.removeIfCause(lit);
        if (_opt_propagator_active)
            _propagator.removeIfCause(lit);
    }
}

void Manager::processUnitsLit() {
    for (const std::unique_ptr<Symmetry>& symmetry : _symmetries) {
	Lit first = symmetry->first();
	if (negate(first) == symmetry->inverse(first)) {
	    if (_lex_order.minimum() == LiteralFalse)
		_unit_clauses.insert(negate(varOf(first)));
	    else
		_unit_clauses.insert(varOf(first));
	}
    }
    Printer::printStat("number of units", _unit_clauses.size());
}

bool Manager::isUnitsLit() {
    return _unit_clauses.size() > 0;
}

Lit Manager::unitLit() {
    assert(_unit_clauses.size() > 0);
    Lit lit = *_unit_clauses.begin();
    _unit_clauses.erase(_unit_clauses.begin());
    return lit;
}

void Manager::order(const std::vector<Lit>& order, LexType lex) {
    _lex_order.assign(order, lex);

    for (const Lit& lit : order) {
        for (unsigned int permutation_index : _watchers[varOf(lit)]) {
            _symmetries[permutation_index]->_lookup_order.push_back(lit);
        }
    }
    processUnitsLit();
}

bool Manager::minimal(Var *cause) {
    if (cause != nullptr)
        *cause = _minimality.cause();
    return _minimality.minimal();
}

void Manager::sbp(std::vector<Lit>* reason) {
    _num_conflicts++;
    return _minimality.sbp(reason);
}

bool Manager::propagate() {
    return _propagator.propagate();
}

void Manager::propagation(Lit *propagate, std::vector<Lit>* reason) {
    _num_propagations++;
    _propagator.propagation(propagate, reason);
}

void Manager::debugPrintPermutations() {
    std::clog << "== Permutations" << std::endl;
    for (unsigned int i = 0; i < _permutations.size(); i++) {
        std::clog << "[" << i << "]: ";
        _permutations[i]->debugPrint();
    }
}

void Manager::debugPrintWatchers() {
    std::clog << "== Watchers" << std::endl;
    // Start index 1 -> 0 is LIT_UNDEF
    for (unsigned int i = 1; i < _watchers.size(); i++) {
        std::clog << "[" << i << "]: ";
        for (const unsigned int permutation_index : _watchers[i])
            std::clog << permutation_index << " ";
        std::clog << std::endl;
    }
}

void Manager::debugPrintSymmetries() {
    std::clog << "== Symmetries" << std::endl;
    for (unsigned int i = 0; i < _symmetries.size(); i++) {
        std::clog << "[" << i << "]: ";
        _symmetries[i]->debugPrint();
    }
}

void Manager::printStats() {
    Printer::printStat("number of esbp", _num_conflicts);
    // if (_opt_propagator_active)
    //     std::cout << "symmetry propagations: " << _num_propagations <<
    //         std::endl;
}
