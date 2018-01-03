
#include "cosy/SymmetryManager.h"
#include "cosy/Printer.h"


static bool SPFSIsActive = true;

namespace cosy {

SymmetryManager::SymmetryManager(unsigned int num_vars,
                                 const PermutationGroup& group) :
        _group(group),
        _order(nullptr) {

    _assignment.resize(num_vars);

    for (const std::unique_ptr<Permutation>& perm : group) {
        const unsigned int index = _statuses.size();

        std::unique_ptr<PermutationStatus> status
            (new PermutationStatus(index, _assignment, _order, perm));
        _statuses.emplace_back(status.release());

        if (SPFSIsActive) {
            std::unique_ptr<PermutationSPFS> spfs
                (new PermutationSPFS(index, _assignment, perm));
            _symmetries_spfs.emplace_back(spfs.release());
        }
    }

}

SymmetryManager::~SymmetryManager() {
}


void SymmetryManager::setOrder(std::unique_ptr<Order> && order) {
    _order = std::move(order);
    DCHECK_NOTNULL(_order);

    for (const std::unique_ptr<PermutationStatus>& status : _statuses)
        status->reset();

    for (const Literal& literal : *_order)
        updateOrderedOrders(literal);

    findUnitClauses();
}


void SymmetryManager::updateOrderedOrders(Literal literal) {
    const BooleanVariable variable = literal.variable();
    for (const unsigned int& index : _group.permutationWith(variable))
        _statuses[index]->addToOrderedSupport(literal);

}

void SymmetryManager::updateNotify(const Literal& literal) {
    DCHECK_NOTNULL(_order);

    _assignment.assignFromTrueLiteral(literal, false); // TODO ISDECISION

    if (_lex_leader.isNotLexLeader())
        return;

    BooleanVariable variable = literal.variable();
    for (const unsigned int& index : _group.permutationWith(variable)) {
        const std::unique_ptr<PermutationStatus>& status = _statuses[index];
        status->updateNotifyLookupVariable(literal);

        if (status->isReducer()) {
            status->generateESBP(literal.variable(), &_lex_leader);
            break;
        }
    }

    if (_lex_leader.isNotLexLeader() || !SPFSIsActive)
        return;

    /* SPFS */
    for (const unsigned int& index : _group.permutationWith(variable)) {
        const std::unique_ptr<PermutationSPFS>& spfs = _symmetries_spfs[index];
        spfs->updateNotify(literal);
    }


}

void SymmetryManager::updateCancel(const Literal& literal) {
    DCHECK_NOTNULL(_order);

    BooleanVariable variable = literal.variable();

    /*
      Update SPFS is done BEFORE unassignLiteral because it check the value
      of unassigned literal
    */

    /* SPFS */
    for (const unsigned int& index : _group.permutationWith(variable)) {
        const std::unique_ptr<PermutationSPFS>& spfs = _symmetries_spfs[index];
        spfs->updateCancel(literal);
    }

    _assignment.unassignLiteral(literal);

    for (const unsigned int& index : _group.permutationWith(variable)) {
        const std::unique_ptr<PermutationStatus>& status = _statuses[index];

        status->updateCancelLookupVariable(literal);
    }

    _lex_leader.removeIfReason(literal);
    _lex_leader_forcing.removeIfReason(literal);
}


bool SymmetryManager::isNotLexLeader(const Literal& literal) const {

    return _lex_leader.isNotLexLeader(literal);
}

const std::unique_ptr<Clause>& SymmetryManager::generateESBP() {
    _stats.esbp.add(1);
    return _lex_leader.esbp();
}



// void SymmetryManager::searchLexLeaderForcing(Literal literal) {
//     BooleanVariable variable = literal.variable();

//     for (const unsigned int& index : _group.permutationWith(variable)) {
//         const std::unique_ptr<PermutationStatus>& status = _statuses[index];
//         if (status->canForceLexLeader(_order, _assignment)) {
//             std::unique_ptr<Clause> esbp =
//                 status->generateForceLexLeaderESBP(variable, _assignment);
//             _lex_leader_forcing.registerESBP(esbp, literal);
//         }
//     }
// }

// bool SymmetryManager::canForceLexLeader(Literal literal) const {
//     return _lex_leader_forcing.isESBPExist(literal);
// }

// std::unique_ptr<Clause> SymmetryManager::generateForceLexLeaderESBP() {
//     _stats.esbp_forcing.add(1);

//     return _lex_leader_forcing.ESBP();
// }



void SymmetryManager::findUnitClauses() {
    Literal literal, inverse;
    for (std::unique_ptr<PermutationStatus>& status : _statuses) {
        if (status->isEndOfOrder())
            continue;
        literal = status->lookupLiteral();
        inverse = status->lookupLiteralInverse();

        if (literal == inverse.negated()) {
            if (_order->valueOrderType() == TRUE_LESS_FALSE) {
                _units_clauses.insert(Literal(literal.variable(), true));
            }
            else if (_order->valueOrderType() == FALSE_LESS_TRUE)
                _units_clauses.insert(Literal(literal.variable(), false));
        }
    }
}


bool SymmetryManager::isUnitsLiteral() const {
    return _units_clauses.size() > 0;
}
Literal SymmetryManager::unitLiteral() {
    DCHECK(isUnitsLiteral());
    Literal unit = *_units_clauses.begin();
    _units_clauses.erase(_units_clauses.begin());

    return unit;
}

void SymmetryManager::debugInactiveStats() const {
    // uint64_t num_inactives = 0, num_valid = 0;
    // for (const std::unique_ptr<PermutationStatus>& status : _statuses) {

    //     if (status->isEndOfOrder())
    //         continue;

    //     if (status->isInactive(_order, _assignment))
    //         num_inactives++;
    //     num_valid++;
    // }
    // Printer::printStat("Number of Inactives", num_inactives, num_valid);
}

void SymmetryManager::debugObservationStats() const {
    // std::unordered_set<BooleanVariable> observe;
    // BooleanVariable var;

    // for (const std::unique_ptr<PermutationStatus>& status : _statuses) {

    //     var = status->lookupVariable();
    //     if (var != kNoBooleanVariable)
    //         observe.insert(var);
    //     var = status->lookupVariableInverse();
    //     if (var != kNoBooleanVariable)
    //         observe.insert(var);
    // }

    // Printer::printStat("Number of observed variable",
    //                    static_cast<uint64_t>(observe.size()),
    //                    _group.numberOfSymmetricVars());
    // std::string observed;
    // int unaffected = 0;

    // for (const BooleanVariable& v : observe) {
    //     if (! _assignment.variableIsAssigned(v)) {
    //         unaffected++;
    //         observed += std::to_string(v.value()) + " ";
    //     }
    // }
    // Printer::printStat("unaffected observed variable",
    //                    static_cast<uint64_t>(unaffected),
    //                    _group.numberOfSymmetricVars());

    //     Printer::print(observed);
}

void SymmetryManager::debugFullObservation() const {
    // std::unordered_set<BooleanVariable> observe;
    // std::vector<BooleanVariable> local;
    // for (const std::unique_ptr<PermutationStatus>& status : _statuses) {

    //     status->observed(&local);
    //     observe.insert(local.begin(), local.end());

    //  }
    // Printer::printStat("Full Number of observed variable",
    //                    static_cast<uint64_t>(observe.size()),
    //                    _group.numberOfSymmetricVars());
}

void SymmetryManager::debugESBPStats() const {
    // _stats.esbp.print();
}

void SymmetryManager::debugPermutationStatusStats() const {
    // std::string label, isInactive;
    // int index = 0;
    // BooleanVariable element, inverse;

    // for (const std::unique_ptr<PermutationStatus>& status :
    //          _statuses) {

    //     element = status->lookupVariable();
    //     inverse = status->lookupVariableInverse();

    //     isInactive = status->isInactive(_order, _assignment) ? "true " : "false";

    //     if (status->isInactive(_order, _assignment)) {
    //         index++;
    //         continue;
    //     }
    //     label = " === [" + std::to_string(index) + "] ";

    //     Printer::print(label);
    //     status->_stats.touched.print();
    //     status->_stats.esbp.print();

    //     Printer::printStat("inactive", isInactive.c_str());
    //     status->_stats.index.print();
    //     Printer::printStat("variable observe", element.value());
    //     Printer::printStat("inverse variable observe", inverse.value());

    //     index++;
    // }
}

void SymmetryManager::tryAddOrder(Literal literal) {
    BooleanVariable variable = literal.variable();
    Literal inverse;


    if (literal.isNegative())
        literal = literal.negated();

    _order->addToOrder(literal);
    updateOrderedOrders(literal);

    const ValueOrderType vot = _order->valueOrderType();

    DCHECK(vot == TRUE_LESS_FALSE || vot == FALSE_LESS_TRUE);

    for (const unsigned int& index : _group.permutationWith(variable)) {
        const std::unique_ptr<PermutationStatus>& status =  _statuses[index];

        inverse = status->inverseOf(literal);
        if (!_order->contains(inverse)) {
            _order->addToOrder(inverse);
            updateOrderedOrders(inverse);
        }
    }
}

}  // namespace cosy
