

#include "cosy/Order.h"

namespace cosy {


/* virtual */ void Order::addToOrder(const Literal& literal) /* final */ {
    DCHECK(_indexes.find(literal) == _indexes.end());
    _indexes[literal] = _order.size();
    _indexes[literal.negated()] = _order.size();

    _order.push_back(literal);
}

/* virtual */ bool Order::contains(const Literal& literal) const {
    return _indexes.find(literal) != _indexes.end();
}

/* virtual */
Literal Order::leq(const Literal &l, const Literal &r) const /* final */ {
    DCHECK(contains(l));
    DCHECK(contains(r));

    return (_indexes.at(l) < _indexes.at(r)) ? l : r;
}



/* virtual */ void Order::debugPrint() const /* final */ {
    std::string order = "order: ";
    for (const Literal& lit : _order)
        order += std::to_string(lit.signedValue()) + " ";
    LOG(INFO) << order;

    // std::string indexes = "indexes: ";
    // for (const auto& p : _indexes)
    //     indexes += std::to_string(p.first.signedValue()) + "(" +
    //         std::to_string(p.second) + ") ";
    // LOG(INFO) << indexes;
}


}  // namespace cosy
