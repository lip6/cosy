/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_SYMMETRYCONTROLLER_H_
#define INCLUDE_DSB_SYMMETRYCONTROLLER_H_

#include <string>
#include <utility>
#include <memory>
#include <vector>


#include "cosy/CNFModel.h"
#include "cosy/CNFReader.h"
#include "cosy/LiteralAdapter.h"
#include "cosy/logging.h"
#include "cosy/Order.h"
#include "cosy/Orbits.h"
#include "cosy/PermutationGroup.h"
#include "cosy/SaucyReader.h"
#include "cosy/SymmetryManager.h"
#include "cosy/Printer.h"

namespace cosy {

template<class T>
class SymmetryController {
 public:
    SymmetryController(const std::string& cnf_filename,
                       const std::string& symmetry_filename,
                       const std::unique_ptr<LiteralAdapter<T>>& adapter);
    ~SymmetryController();

    bool isValid() const;

    void order(VariableOrderType variableOrderType,
               ValueOrderType valueOderType);

    void order(std::vector<T> order, ValueOrderType valueOderType);


    void updateNotify(T lit, unsigned int level, bool isDecision);
    void updateCancel(T lit);
    void notifyOrderAnalysis();

    bool isUnitsLit();
    T unitLit();

    bool isNotLexLeader(T propagated);
    std::vector<T> generateESBP();

    /* SPFS */
    bool canSPFSPropagate(T *propagate);
    std::vector<T> generateSPFSClause(const std::vector<T>& reason);
    // void activateLexLeaderForcing();
    // bool canForceLexLeader(T propagate);
    // std::vector<T>generateForceLexLeaderESBP();

    void printInfo();
    void printSubStats() const;

    void printStats();

 private:
    unsigned int _num_vars;
    const std::unique_ptr<LiteralAdapter<T>>& _literal_adapter;
    std::unique_ptr<SymmetryManager> _symmetry_manager;
    CNFModel _model;
    PermutationGroup _group;
    Orbits _orbits;
    Order* _order;
};

template<class T>
inline SymmetryController<T>::SymmetryController(
                           const std::string& cnf_filename,
                           const std::string& symmetry_filename,
                           const std::unique_ptr<LiteralAdapter<T>>& adapter) :
    _literal_adapter(adapter),
    _symmetry_manager(nullptr),
    _order(nullptr) {

    CNFReader cnf_reader;
    SaucyReader symmetry_reader;
    bool success;

    success = cnf_reader.load(cnf_filename, &_model);
    if (!success)
        LOG(FATAL) << "Cannot load cnf model" << std::endl;

    _num_vars = _model.numberOfVariables();

    success = symmetry_reader.load(_num_vars, symmetry_filename, &_group);
    if (!success)
        LOG(FATAL) << "Cannot load symmetry file" << std::endl;

    if  (_group.numberOfPermutations() == 0)
        return;

    _orbits.assign(_group.permutations());

    _symmetry_manager = std::unique_ptr<SymmetryManager>
        (new SymmetryManager(_num_vars, _group));
}

template<class T>
inline SymmetryController<T>::~SymmetryController() {
}


template<class T>
inline void SymmetryController<T>::order(VariableOrderType variableOrderType,
                                         ValueOrderType valueOderType) {

    std::unique_ptr<Order> order = std::unique_ptr<Order>
        (Order::create(_num_vars, variableOrderType, valueOderType,
                       _model, _group));
    DCHECK_NOTNULL(order);

    _order = order.get();
    _symmetry_manager->setOrder(std::move(order));
}

template<class T>
inline void SymmetryController<T>::order(std::vector<T> lits,
                                         ValueOrderType valueOrderType) {

    cosy::Literal l;
    std::vector<Literal> literals;
    for (const T& lit : lits)
        literals.push_back( _literal_adapter->convertTo(lit));
    std::unique_ptr<Order> order = std::unique_ptr<Order>
        (Order::create(_num_vars, literals, valueOrderType));

    _order = order.get();
    _symmetry_manager->setOrder(std::move(order));

}



template<class T>
inline void SymmetryController<T>::updateNotify(T lit, unsigned int level,
                                                bool isDecision) {
    cosy::Literal l = _literal_adapter->convertTo(lit);
    _symmetry_manager->updateNotify(l, level, isDecision);
}

template<class T>
inline void SymmetryController<T>::updateCancel(T lit) {
    cosy::Literal l = _literal_adapter->convertTo(lit);
    _symmetry_manager->updateCancel(l);
}


template<class T>
inline void SymmetryController<T>::printSubStats() const {
}

template<class T>
inline bool SymmetryController<T>::isUnitsLit() {
    return _symmetry_manager->isUnitsLiteral();
}

template<class T>
inline T SymmetryController<T>::unitLit() {
    cosy::Literal l = _symmetry_manager->unitLiteral();
    T lit =  _literal_adapter->convertFrom(l);
    return lit;
}

template<class T>
inline bool SymmetryController<T>::isNotLexLeader(T propagated) {
    cosy::Literal l = _literal_adapter->convertTo(propagated);
    return _symmetry_manager->isNotLexLeader(l);
}


template<class T>
inline bool SymmetryController<T>::canSPFSPropagate(T *propagate) {
    cosy::Literal l;
    bool result = _symmetry_manager->canSPFSPropagate(&l);
    if (result)
        *propagate = _literal_adapter->convertFrom(l);
    return result;
}

template<class T> inline std::vector<T>
SymmetryController<T>::generateSPFSClause(const std::vector<T>& reason) {
    std::vector<T> implication;
    std::vector<cosy::Literal> implication_cosy;
    std::vector<cosy::Literal> reason_cosy;

    for (const T& l : reason)
        reason_cosy.push_back(_literal_adapter->convertTo(l));

    _symmetry_manager->generateSymmetricClause(reason_cosy, &implication_cosy);

    for (const cosy::Literal l : implication_cosy)
        implication.push_back(_literal_adapter->convertFrom(l));

    return implication;
}


template<class T>
inline std::vector<T> SymmetryController<T>::generateESBP() {
    std::vector<T> esbp;
    T lit;

    const std::unique_ptr<Clause>& clause = _symmetry_manager->generateESBP();

    for (const cosy::Literal& l : *clause) {
        lit = _literal_adapter->convertFrom(l);
        esbp.push_back(lit);
    }

    return esbp;
}

// /* With Force Lex leader */
// template<class T>
// inline void SymmetryController<T>::activateLexLeaderForcing() {
//     _manager->activatePropagations();
// }

// template<class T>
// inline bool SymmetryController<T>::canForceLexLeader(T propagated) {
//     cosy::Literal l = _literal_adapter->convertTo(propagated);
//     return _symmetry_manager->canForceLexLeader(l);
// }

// template<class T>
// inline std::vector<T>
// SymmetryController<T>::generateForceLexLeaderESBP() {
//     std::unique_ptr<Clause> clause = nullptr;
//     std::vector<T> esbp;
//     T lit;
//     std::string clause_str = "";

//     clause = _symmetry_manager->generateForceLexLeaderESBP();
//     for (const cosy::Literal& l : *clause) {
//         clause_str += std::to_string(l.signedValue()) + " ";

//         lit = _literal_adapter->convertFrom(l);
//         esbp.push_back(lit);
//     }
//     LOG(INFO) << clause_str;

//     return esbp;
// }


/* Some prints */
template<class T>
inline void SymmetryController<T>::printInfo() {
    const int korderElement = 10;
    std::string order = "";
    int i = 0;
    for (const Literal& literal : _order->order()) {
        order += std::to_string(literal.signedValue()) + " ";
        if (++i == korderElement) {
            order += "...";
            break;
        }
    }

    Printer::printSection(" Symmetry Informations ");
    Printer::printStat("Variable Order", _order->variableInfo());
    Printer::printStat("Value Order", _order->valueInfo());
    Printer::printStat("Order", order);
    Printer::printStat("Number of generators", _group.numberOfPermutations());
    Printer::printStat("Number of orbits", _orbits.numberOfOrbits());
    Printer::printStat("Number of vars in generators",
                       _group.numberOfSymmetricVars(),
                       static_cast<uint64_t>(_num_vars));
    Printer::print("");


    Printer::printSection(" Instance Informations ");
    Printer::printStat("Number of variables", _model.numberOfVariables());
    Printer::printStat("Number of clauses", _model.numberOfClauses());
    Printer::printStat(" |- unary clauses", _model.numberOfUnaryClauses(),
                       _model.numberOfClauses());
    Printer::printStat(" |- binary clauses", _model.numberOfBinaryClauses(),
                       _model.numberOfClauses());
    Printer::printStat(" |- ternary clauses", _model.numberOfTernaryClauses(),
                       _model.numberOfClauses());
    Printer::printStat(" |- other clauses", _model.numberOfOtherClauses(),
                       _model.numberOfClauses());

    Printer::printSection(" Symmetry Solve Informations ");
    Printer::printStat("Number of units (cause inverting)",
                       _symmetry_manager->numberOfUnits());

}

template<class T>
inline void SymmetryController<T>::printStats() {
    _symmetry_manager->printStats();
}

}  // namespace cosy

#endif  // INCLUDE_DSB_SYMMETRYCONTROLLER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
