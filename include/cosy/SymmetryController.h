/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_SYMMETRYCONTROLLER_H_
#define INCLUDE_DSB_SYMMETRYCONTROLLER_H_

#include <string>
#include <utility>
#include <memory>
#include <vector>

#include "cosy/OrderManager.h"
#include "cosy/Types.h"
#include "cosy/Manager.h"
#include "cosy/Parser.h"
#include "cosy/ParserSaucy.h"
#include "cosy/LiteralAdapter.h"
#include "cosy/LexOrder.h"
#include "cosy/Printer.h"
#include "cosy/CNFParser.h"

namespace cosy {

template<class T>
class SymmetryController {
 public:
        SymmetryController(const unsigned int num_vars,
                    const std::unique_ptr<LiteralAdapter<T>>& literal_adapter);
        ~SymmetryController();

        bool initialize(const std::string cnf_file,
                        const std::string symmetry_file);

        void order(const std::vector<T>& order, LexType lex);
        void order(OrderType order, LexType lex);

        void updateNotify(T lit, unsigned int level);
        void updateCancel(T lit);

        bool isNotLexLeader(T propagated);
        std::vector<T>generateEsbp();

        void activateLexLeaderForcing();
        bool canForceLexLeader();
        std::vector<T>generateForceLexLeaderEsbp(T* propagate);

        void printInfo();
        void printStats();

 private:
        const unsigned int _num_vars;
        const std::unique_ptr<LiteralAdapter<T>>& _literal_adapter;
        Orbits _orbits;
        CNFModel _model;
        std::unique_ptr<Manager> _manager;
        std::unique_ptr<OrderManager> _order_manager;
};

template<class T>
inline SymmetryController<T>::SymmetryController(const unsigned int num_vars,
                   const std::unique_ptr<LiteralAdapter<T>>& literal_adapter) :
    _num_vars(num_vars),
    _literal_adapter(literal_adapter),
    _manager(new Manager(num_vars)),
    _order_manager(new OrderManager(num_vars)) {
}

template<class T>
inline SymmetryController<T>::~SymmetryController() {
}

/* Initialisation */
template<class T>
inline bool SymmetryController<T>::initialize(const std::string cnf_file,
                                           const std::string symmetry_file) {
    std::unique_ptr<Parser> sym_parser = nullptr;
    std::vector< std::unique_ptr<Permutation> > permutations;
    CNFParser cnf_parser;

    if (symmetry_file.empty())
        return false;

    _model = cnf_parser.parse(cnf_file);

    sym_parser = std::unique_ptr<ParserSaucy>(new ParserSaucy());
    permutations = sym_parser->parse(_num_vars, symmetry_file);

    _orbits.compute(permutations);

    _order_manager->orbits() = _orbits.orbits();
    _order_manager->occurences() = _model.occurences();

    for (std::unique_ptr<Permutation>& permutation : permutations)
        _manager->addPermutation(std::move(permutation));

    return _manager->numGenerators() > 0;
}

/* Configuration */
template<class T>
inline void SymmetryController<T>::order(const std::vector<T>& order, LexType lex) {
    std::vector<Lit> cosy_order;
    cosy::Lit lit;

    for (const T& l : order) {
        lit = _literal_adapter->convert(l);
        cosy_order.push_back(lit);
    }
    _order_manager->custom(cosy_order);

    /* TODO : put lex order type in parameter */
    _manager->order(cosy_order, lex);
}

template<class T>
inline void SymmetryController<T>::order(OrderType order, LexType lex) {
    _order_manager->generate(order);
    _manager->order(_order_manager->order(), lex);
}

/* Solve API */

template<class T>
inline void SymmetryController<T>::updateNotify(T lit, unsigned int level) {
    cosy::Lit l = _literal_adapter->convert(lit);
    _manager->notify(l, level);
}

template<class T>
inline void SymmetryController<T>::updateCancel(T lit) {
    cosy::Lit l = _literal_adapter->convert(lit);
    _manager->cancel(l);
}

template<class T>
inline bool SymmetryController<T>::isNotLexLeader(T propagated) {
    cosy::Lit l = _literal_adapter->convert(propagated);
    cosy::Var v = varOf(l);
    cosy::Var cause;
    bool minimal;

    minimal = _manager->minimal(&cause);
    if (minimal)
        return false;
    if (cause != v)
        return false;
    return true;
}

template<class T>
inline std::vector<T> SymmetryController<T>::generateEsbp() {
    std::vector<cosy::Lit> r;
    std::vector<T> esbp;
    T lit;

    _manager->sbp(&r);
    for (const cosy::Lit l : r) {
        lit = _literal_adapter->convert(l);
        esbp.push_back(lit);
    }

    return esbp;
}

/* With Force Lex leader */
template<class T>
inline void SymmetryController<T>::activateLexLeaderForcing() {
    _manager->activatePropagations();
}

template<class T>
inline bool SymmetryController<T>::canForceLexLeader() {
    return _manager->propagate();
}

template<class T>
inline std::vector<T>
SymmetryController<T>::generateForceLexLeaderEsbp(T* propagate) {
    cosy::Lit l;
    std::vector<cosy::Lit> r;
    T lit;
    std::vector<T> reason;

    _manager->propagation(&l, &r);

    if (propagate != nullptr)
        *propagate = _literal_adapter->convert(l);

    for (const cosy::Lit l : r) {
        lit = _literal_adapter->convert(l);
        reason.push_back(lit);
    }
    return reason;
}


/* Some prints */
template<class T>
inline void SymmetryController<T>::printInfo() {
    Printer::printSection(" Symmetry Informations ");
    Printer::printStat("Variable Order", _order_manager->orderString());
    Printer::printStat("Value Order", _manager->lexOrder().orderString());
    Printer::printStat("Number of generators", _manager->numGenerators());
    Printer::printStat("Number of orbits", _orbits.numOrbits());
    Printer::printStat("Number of vars in generators",
                       _manager->numSymmetricVars(),
                       static_cast<uint64_t>(_num_vars));
    Printer::print("");

    Printer::printSection(" Instance Informations ");
    _model.printStats();
}

template<class T>
inline void SymmetryController<T>::printStats() {
    Printer::printSection(" Symmetry Statistics ");
    Printer::printStat("Number of esbp", _manager->numConflicts());
    Printer::print("");
}

}  // namespace cosy

#endif  // INCLUDE_DSB_SYMMETRYCONTROLLER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
