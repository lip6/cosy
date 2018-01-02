// Copyright 2017 Hakan Metin

#ifndef SAT_CORE_LITERAL_H_
#define SAT_CORE_LITERAL_H_

#include <functional>
#include <string>

#include "cosy/logging.h"
#include "cosy/int_type.h"

namespace cosy {

// Index of a variable (>= 0).
DEFINE_INT_TYPE(BooleanVariable, int);
const BooleanVariable kNoBooleanVariable(-1);

// Index of a literal (>= 0), see Literal below.
DEFINE_INT_TYPE(LiteralIndex, int);
const LiteralIndex kNoLiteralIndex(-1);

// Special values used in some API to indicate a literal that is always true
// or always false.

const LiteralIndex kTrueLiteralIndex(-2);
const LiteralIndex kFalseLiteralIndex(-3);

// A literal is used to represent a variable or its negation. If it represents
// the variable it is said to be positive. If it represent its negation, it is
// said to be negative. We support two representations as an integer.
//
// The "signed" encoding of a literal is convenient for input/output and is used
// in the cnf file format. For a 0-based variable index x, (x + 1) represent the
// variable x and -(x + 1) represent its negation. The signed value 0 is an
// undefined literal and this class can never contain it.
//
// The "index" encoding of a literal is convenient as an index to an array
// and is the one used internally for efficiency. It is always positive or zero,
// and for a 0-based variable index x, (x << 1) encode the variable x and the
// same number XOR 1 encode its negation.
class Literal {
 public:
    // Not explicit for tests so we can write:
    // std::vector<literal> literal = {+1, -3, +4, -9};
    Literal(int signed_value)  // NOLINT
        : _index(signed_value > 0 ? ((signed_value - 1) << 1)
                 : ((-signed_value - 1) << 1) ^ 1) {
        CHECK_NE(signed_value, 0);
    }

    Literal() {}
    explicit Literal(LiteralIndex index) : _index(index.value()) {}
    Literal(BooleanVariable variable, bool is_positive)
        : _index(is_positive ? (variable.value() << 1)
                 : (variable.value() << 1) ^ 1) {}

    BooleanVariable variable() const { return BooleanVariable(_index >> 1); }
    bool isPositive() const { return !(_index & 1); }
    bool isNegative() const { return (_index & 1); }

    LiteralIndex index() const { return LiteralIndex(_index); }
    LiteralIndex negatedIndex() const { return LiteralIndex(_index ^ 1); }

    int signedValue() const {
        return (_index & 1) ? -((_index >> 1) + 1) : ((_index >> 1) + 1);
    }

    Literal negated() const { return Literal(negatedIndex()); }

    std::string debugString() const { return std::string("" + signedValue()); }
    bool operator==(Literal other) const { return _index == other._index; }
    bool operator!=(Literal other) const { return _index != other._index; }


    bool operator<(const Literal& literal) const {
        return index() < literal.index();
    }

 private:
    int _index;
};

}  // namespace cosy

 namespace std {
 template <>
     struct hash<cosy::Literal> {
     size_t operator()(const cosy::Literal& literal) const {
         return literal.index().value();
     }
 };
 }  // namespace std


#endif  // SAT_CORE_LITERAL_H_
