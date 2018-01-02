#include "cosy/PermutationStatus.h"

namespace cosy {

PermutationStatus::PermutationStatus(const unsigned int permutation_index,
                                     const Assignment& assignment,
                                     const std::unique_ptr<Order>& order,
                            const std::unique_ptr<Permutation>& permutation) :
    _permutation_index(permutation_index),
    _assignment(assignment),
    _order(order),
    _status(ACTIVE),
    _lookup_index(0) {

    Literal element;
    for (unsigned int c = 0; c < permutation->numberOfCycles(); c++) {
        element = permutation->lastElementInCycle(c);
        for (const Literal& image : permutation->cycle(c)) {

            _inverse[image] = element;
            element = image;
        }
    }
}

PermutationStatus::~PermutationStatus() {
}


void PermutationStatus::addToOrderedSupport(const Literal& literal) {
    _ordered_support.push_back(literal);
}

bool PermutationStatus::isEndOfOrder() const {
    return _lookup_index >= _ordered_support.size();
}

Literal PermutationStatus::inverseOf(const Literal& literal) const {
    DCHECK(_inverse.find(literal) != _inverse.end());
    return _inverse.at(literal);
}

Literal PermutationStatus::lookupLiteral() const {
    DCHECK(!isEndOfOrder());
    return _ordered_support[_lookup_index];
}

Literal PermutationStatus::lookupLiteralInverse() const {
    DCHECK(!isEndOfOrder());
    return inverseOf(_ordered_support[_lookup_index]);
}


BooleanVariable PermutationStatus::lookupVariable() const {
    return lookupLiteral().variable();
}

BooleanVariable PermutationStatus::lookupVariableInverse() const {
    return lookupLiteralInverse().variable();
}

bool PermutationStatus::updateNotifyLookupVariable(Literal literal) {
    Literal element, inverse;
    unsigned int initial = _lookup_index;

    BooleanVariable variable = literal.variable();
    if (variable != lookupVariable() && variable != lookupVariableInverse())
        return false;

    for (; _lookup_index < _ordered_support.size(); _lookup_index++) {
        element = _ordered_support[_lookup_index];
        inverse = inverseOf(element);

        if (!_assignment.hasSameAssignmentValue(element, inverse))
            break;

        if (_lookup_index == initial)
            _trail_index.push_back(Lookup(literal.variable(), initial));
    }

    _status = updateStatus();

    return initial != _lookup_index;
}

bool PermutationStatus::updateCancelLookupVariable(Literal literal) {
    unsigned int initial = _lookup_index;

    while (_trail_index.size() > 0) {
        const Lookup& lookup = _trail_index.back();

        if (_assignment.variableIsAssigned(lookup.var))
            break;

        _lookup_index = lookup.index;
        _trail_index.pop_back();
    }

    if (_status != PERMANENTLY_INACTIVE)
        _status = ACTIVE;

    return initial != _lookup_index;
}

Status PermutationStatus::updateStatus() {
    if (_status == PERMANENTLY_INACTIVE)
        return PERMANENTLY_INACTIVE;

    if (isEndOfOrder())
        return ACTIVE;

    Literal element = lookupLiteral();
    Literal inverse = lookupLiteralInverse();

    if (!_assignment.bothLiteralsAreAssigned(element, inverse))
        return ACTIVE;

    if (_assignment.hasSameAssignmentValue(element, inverse))
        debugPrint();
    DCHECK(!_assignment.hasSameAssignmentValue(element, inverse));

    /* inverting case */
    if (element == inverse.negated()) {
        BooleanVariable var = element.variable();
        Literal lit = _assignment.getTrueLiteralForAssignedVariable(var);

        if (_order->valueOrderType() == TRUE_LESS_FALSE)
            CHECK(lit.isPositive());
        else if (_order->valueOrderType() == FALSE_LESS_TRUE)
            CHECK(lit.isNegative());

        return PERMANENTLY_INACTIVE;
    }

    Literal minimal = _order->leq(element, inverse);
    if (_order->valueOrderType() == TRUE_LESS_FALSE) {
        if (_assignment.literalIsFalse(minimal))
            return REDUCER;
        else
            return INACTIVE;
    } else if (_order->valueOrderType() == FALSE_LESS_TRUE) {
        if (_assignment.literalIsTrue(minimal))
            return REDUCER;
        else
            return INACTIVE;
    }
    CHECK(false);  // order must have one one two value

    return ACTIVE; // dead code - make compiler happy.
}

bool PermutationStatus::isActive() const {
    return _status == ACTIVE;
}

bool PermutationStatus::isReducer() const {
    return _status == REDUCER;
}
bool PermutationStatus::isInactive() const {
    return _status == INACTIVE || _status == PERMANENTLY_INACTIVE;
}

void PermutationStatus::reset() {
    _status = ACTIVE;
    _lookup_index = 0;
    _ordered_support.clear();
    _trail_index.clear();
}


void PermutationStatus::generateESBP(BooleanVariable reason, LexLeader *lex) {
    std::vector<Literal> literals;
    Literal element, inverse, lit;
    std::unordered_set<Literal> used;

    DCHECK(!isEndOfOrder());

    lit = _assignment.getFalseLiteralForAssignedVariable(reason);
    literals.push_back(lit);
    used.insert(lit);

    for (unsigned int i = 0; i <= _lookup_index; i++) {
        element = _ordered_support[i];
        inverse = _inverse.at(element);

        DCHECK(_assignment.bothLiteralsAreAssigned(element, inverse));

        lit = _assignment.getFalseLiteralForAssignedVariable(element.variable());
        if (used.find(lit) == used.end()) {
            literals.push_back(lit);
            used.insert(lit);
        }

        lit = _assignment.getFalseLiteralForAssignedVariable(inverse.variable());
        if (used.find(lit) == used.end()) {
            literals.push_back(lit);
            used.insert(lit);
        }
    }

    DCHECK(literals.size() >= 2);

    std::swap(literals[0], literals[1]);
    lex->add(literals, reason);
}


void
PermutationStatus::observed(std::unordered_set<BooleanVariable> *look) const {
    Literal literal;

    if (isEndOfOrder())
        return;

    for (unsigned int i=0; i <= _lookup_index; i++) {
        literal = _ordered_support[i];
        look->insert(literal.variable());
        literal = inverseOf(literal);
        look->insert(literal.variable());
    }
}

// bool PermutationStatus::canForceLexLeader() {

    // if (!isValid())
    //     return false;

    // Literal element = lookupLiteral();
    // Literal inverse = lookupLiteralInverse();

    // if (assignment.bothLiteralsAreAssigned(element, inverse))
    //     return false;

    // Literal min = order->leq(element, inverse);
    // Literal max = (min == element) ? inverse : element;

    // DCHECK(min != max);

    // if (order->valueOrderType() == TRUE_LESS_FALSE)
    //     return
    //      (!assignment.literalIsAssigned(min) && assignment.literalIsTrue(max)) ||
    //      (assignment.literalIsFalse(min) && !assignment.literalIsAssigned(max));
    // else if (order->valueOrderType() == FALSE_LESS_TRUE)
    //     return
    //     (!assignment.literalIsAssigned(min) && assignment.literalIsFalse(max)) ||
    //     (assignment.literalIsTrue(min) && !assignment.literalIsAssigned(max));

    // DCHECK(false);
    // return false;
// }





// std::unique_ptr<Clause>
// PermutationStatus::generateForceLexLeaderESBP(BooleanVariable reason,
//                                               const Assignment& assignment) {
    // std::vector<Literal> literals;
    // std::unique_ptr<Clause> esbp = nullptr;
    // Literal element, inverse, lit;
    // std::unordered_set<Literal> used;

    // DCHECK(isValid());

    // element = lookupLiteral();
    // inverse = lookupLiteralInverse();

    // Literal undef = (!assignment.literalIsAssigned(element)) ? element : inverse;
    // Literal affected = (undef == inverse) ? element : inverse;

    // DCHECK(undef != affected);

    // lit = Literal(undef.variable(), assignment.literalIsTrue(affected));
    // literals.push_back(lit);
    // used.insert(lit);

    // lit = assignment.getFalseLiteralForAssignedVariable(reason);
    // literals.push_back(lit);
    // used.insert(lit);

    // lit = assignment.getFalseLiteralForAssignedVariable(affected.variable());
    // if (used.find(lit) == used.end()) {
    //     literals.push_back(lit);
    //     used.insert(lit);
    // }

    // for (unsigned int i = 0; i < _minimal_index; i++) {
    //     element = _ordered_support[i];
    //     inverse = _inverse.at(element);

    //     lit = assignment.getFalseLiteralForAssignedVariable(element.variable());
    //     if (used.find(lit) == used.end()) {
    //         literals.push_back(lit);
    //         used.insert(lit);
    //     }

    //     lit = assignment.getFalseLiteralForAssignedVariable(inverse.variable());
    //     if (used.find(lit) == used.end()) {
    //         literals.push_back(lit);
    //         used.insert(lit);
    //     }
    // }

    // esbp = std::unique_ptr<Clause>(new Clause(literals));
    // return esbp;
// }


// void PermutationStatus::observed(std::vector<BooleanVariable> *variables) const {
//     variables->clear();
//     unsigned int size = _ordered_support.size() - 1;
//     unsigned int sz = std::min(_minimal_index, size);
//     for (unsigned int i = 0; i <= sz; i++) {
//         variables->push_back(_ordered_support[i].variable());
//         variables->push_back(inverseOf(_ordered_support[i]).variable());
//     }
// }

void PermutationStatus::debugPrint() const {
    std::string str;
    Literal element, inverse;

    str += "index " + std::to_string(_lookup_index) + " ";
    str += "status " + std::to_string(_status) + " | ";

    for (const Literal& literal : _ordered_support) {
        str += std::to_string(literal.signedValue()) + "(" +
            (_assignment.literalIsTrue(literal)?"T":"F") + ") ";
    }

    str += " | ";
    for (const Literal& literal : _ordered_support) {
        str += std::to_string(inverseOf(literal).signedValue()) + " ";
    }
    LOG(INFO) << str;
}



}  // namespace cosy
