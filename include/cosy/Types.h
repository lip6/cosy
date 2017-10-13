/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_TYPES_H_
#define INCLUDE_DSB_TYPES_H_

#include <cmath>
#include <vector>

namespace cosy {

typedef int Lit;
typedef unsigned int Var;
typedef std::vector<Lit> Clause;

const int LEVEL_UNDEF = -1;

const Lit LIT_UNDEF = 0;
const Var VAR_UNDEF = 0;

enum LiteralValue {
    LiteralFalse = 0,
    LiteralTrue  = 1,
    LiteralUndef = 2
};

static inline Lit createLiteral(Var var, bool sign) {
    return (sign ? -var : var);
}

static inline LiteralValue createLiteralValue(bool b) {
    if (b)
        return LiteralTrue;
    return LiteralFalse;
}

static inline bool sign(Lit lit) {
    return lit < 0;
}

static inline Lit negate(Lit lit) {
    return -lit;
}

static inline Var varOf(Lit lit) {
    return std::abs(lit);
}

}  // namespace cosy
#endif  // INCLUDE_DSB_TYPES_H_
