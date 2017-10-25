/* Copyright 2017 Hakan Metin - All rights reserved*/

#include "cosy/Symmetry.h"

using cosy::Symmetry;

Symmetry::Symmetry(const Assigns& assigns,
                   const unsigned int permutation_index) :
    _assigns(assigns),
    _permutation_index(permutation_index),
    _lookup_index(0),
    _blocked_lit(LIT_UNDEF),
    _num_conflicts(0) {
}

Symmetry::~Symmetry() {
}

bool Symmetry::inactive() const {
    return _blocked_lit != LIT_UNDEF;
}

void Symmetry::forwardLookupIndex(Lit lit) {
    Lit element, inverse;
    LiteralValue element_value, inverse_value;
    if (_lookup_index == _lookup_order.size())
        return;

    element = _lookup_order[_lookup_index];
    element_value = _assigns.valueOf(element);
    inverse = _inverse[element];
    inverse_value = _assigns.valueOf(inverse);

    if (element_value == LiteralUndef || inverse_value == LiteralUndef)
        return;
    if (element_value != inverse_value)
        return;
    if (element_value == inverse_value) {
        _lookup_trail.push_back(LookupInfo(varOf(lit), _lookup_index));
    }

    do {
        _lookup_index++;
        if (_lookup_index == _lookup_order.size())
            return;

        element = _lookup_order[_lookup_index];
        element_value = _assigns.valueOf(element);
        inverse = _inverse[element];
        inverse_value = _assigns.valueOf(inverse);
    } while (element_value != LiteralUndef && element_value == inverse_value);
}

void Symmetry::backwardLookupIndex(Lit lit) {
    if (_blocked_lit == lit)
        _blocked_lit = LIT_UNDEF;

    if (_lookup_trail.empty())
        return;
    const LookupInfo& lookupInfo = _lookup_trail.back();
    if (varOf(lit) == lookupInfo.var) {
        _lookup_index = lookupInfo.lookup_index;
        _lookup_trail.pop_back();
    }
}

void Symmetry::verify(Lit lit, Minimality* minimality, const LexOrder& lex) {
    if (_lookup_index == _lookup_order.size())
        return;

    const Lit element =  _lookup_order[_lookup_index];
    const LiteralValue  element_value = _assigns.valueOf(element);
    const Lit inverse = _inverse.at(element);
    const LiteralValue inverse_value = _assigns.valueOf(inverse);

    if (element_value == LiteralUndef || inverse_value == LiteralUndef)
        return;

    assert(element_value != inverse_value);
    _blocked_lit = lit;

    if (lex.leq(element, inverse) && lex.leq(element_value, inverse_value))
        return;
    if (lex.leq(inverse, element) && lex.leq(inverse_value, element_value))
        return;

    generateSBP(lit, minimality);
}

void Symmetry::generateSBP(Lit lit, Minimality* minimality) {
    std::unordered_set<Var> exists;
    std::vector<Lit> reason;
    Lit l;
    Var v;

    if (_assigns.valueOf(lit) == LiteralTrue)
        reason.push_back(negate(lit));
    else
        reason.push_back(lit);
    exists.insert(varOf(lit));
    for (unsigned int i = 0; i <= _lookup_index; i++) {
        l = _lookup_order[i];
        v = varOf(l);
        if (exists.find(v) == exists.end()) {
            if (_assigns.valueOf(l) == LiteralTrue)
                reason.push_back(negate(l));
            else
                reason.push_back(l);
        }
        exists.insert(v);
        l = _inverse.at(l);
        v = varOf(l);
        if (exists.find(v) == exists.end()) {
            if (_assigns.valueOf(l) == LiteralTrue)
                reason.push_back(negate(l));
            else
                reason.push_back(l);
        }
        exists.insert(v);
    }
    if (reason.size() > 1)
        std::swap(reason[0], reason[1]);

    minimality->add(lit, reason);
    _num_conflicts++;
}

void Symmetry::verify(Lit lit, Propagator* propagator, const LexOrder& lex) {
    if (_lookup_index == _lookup_order.size())
        return;

    const Lit element =  _lookup_order[_lookup_index];
    const LiteralValue  element_value = _assigns.valueOf(element);

    const Lit inverse = _inverse.at(element);
    const LiteralValue inverse_value = _assigns.valueOf(inverse);

    const Var propagated = varOf(lit);
    bool element_leq_inverse;
    assert(element != LIT_UNDEF);
    assert(inverse != LIT_UNDEF);

    assert(element != inverse);
    if (propagated != varOf(element) && propagated != varOf(inverse))
        return;
    if (element_value != LiteralUndef && inverse_value != LiteralUndef)
        return;

    element_leq_inverse = lex.leq(element, inverse);
    if ((element_leq_inverse && element_value == lex.maximum()) ||
        (!element_leq_inverse && element_value == lex.minimum())) {
        generateSP(inverse, element, propagator);
    } else if ((element_leq_inverse && inverse_value == lex.minimum()) ||
               (!element_leq_inverse && inverse_value == lex.maximum())) {
        generateSP(element, inverse, propagator);
    }
}

void Symmetry::generateSP(Lit undef, Lit cause, Propagator* propagator) {
    std::unordered_set<Var> exists;
    Lit propagate;
    std::vector<Lit> reason;
    Lit l;
    Var v;

    assert(_assigns.valueOf(undef) == LiteralUndef);
    assert(_assigns.valueOf(cause) != LiteralUndef);

    if (_assigns.valueOf(cause) == LiteralFalse)
        propagate = negate(undef);
    else
        propagate = undef;

    reason.push_back(propagate);
    exists.insert(varOf(propagate));

    for (unsigned int i = 0; i <= _lookup_index; i++) {
        l = _lookup_order[i];
        v = varOf(l);
        if (exists.find(v) == exists.end()) {
            assert(_assigns.valueOf(v) != LiteralUndef);
            if (_assigns.valueOf(l) == LiteralTrue)
                reason.push_back(negate(l));
            else
                reason.push_back(l);
        }
        exists.insert(v);
        l = _inverse.at(l);
        v = varOf(l);

        if (exists.find(v) == exists.end()) {
            assert(_assigns.valueOf(v) != LiteralUndef);
            if (_assigns.valueOf(l) == LiteralTrue)
                reason.push_back(negate(l));
            else
                reason.push_back(l);
        }
        exists.insert(v);
    }
    propagator->add(propagate, cause, reason);
}

void Symmetry::debugPrint() {
    std::clog  // << "[" << _permutation_index << "]: "
        << "inactive: " << inactive()
        << " | index: " << _lookup_index
        << " | confl: " << _num_conflicts
        << " | order: ";
    // for (unsigned int i = 0; i < _lookup_order.size(); i++)
    //     std::clog << _lookup_order[i] << " ";
    std::clog << std::endl;
}
