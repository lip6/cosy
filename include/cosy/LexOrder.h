/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_LEXORDER_H_
#define INCLUDE_DSB_LEXORDER_H_

#include <unordered_map>
#include <cassert>
#include <vector>
#include <string>

#include "cosy/Types.h"

namespace cosy {

enum LexType {
    F_LESS_T = 0,
    T_LESS_F = 1,
    UNKNOWN_LEX = 2
};

class LexOrder {
 public:
        LexOrder();
        ~LexOrder();

        bool affected() const { return _lex != UNKNOWN_LEX; }

        void assign(const std::vector<Lit>& order, LexType lex);

        bool leq(const LiteralValue a, const LiteralValue b) const;
        bool leq(const Lit a, const Lit b) const;

        LiteralValue minimum() const { return _minimum; }
        LiteralValue maximum() const { return _maximum; }

        std::string orderString() const {
            std::string min = (_minimum == LiteralTrue) ? "True" :
                (_minimum == LiteralFalse) ? "False" : "Undef";
            std::string max = (_maximum == LiteralTrue) ? "True" :
                (_maximum == LiteralFalse) ? "False" : "Undef";

            return std::string(min + " < " + max);
        }

 private:
        LexType _lex;
        std::unordered_map<Lit, unsigned int> _order;
        LiteralValue _minimum;
        LiteralValue _maximum;
};

inline LexOrder::LexOrder() :
    _lex(UNKNOWN_LEX),
    _minimum(LiteralUndef),
    _maximum(LiteralUndef) {
}

inline LexOrder::~LexOrder() {
}


inline void LexOrder::assign(const std::vector<Lit>& order, LexType lex) {
    int index;

    _lex = lex;

    for (const Lit& lit : order) {
        assert(_order.find(lit) == _order.end());
        index = _order.size();
        _order[lit] = index;
    }

    if (_lex == T_LESS_F) {
        _minimum = LiteralTrue;
        _maximum = LiteralFalse;
    } else if (_lex == F_LESS_T) {
        _minimum = LiteralFalse;
        _maximum = LiteralTrue;
    } else {
        // Cannot be other thing
        assert(false);
    }
}

inline bool LexOrder::leq(const LiteralValue a, const LiteralValue b) const {
    assert(a != LiteralUndef);
    assert(b != LiteralUndef);
    assert(_lex != UNKNOWN_LEX);

    if (_lex == T_LESS_F)
        return b < a;
    else if (_lex == F_LESS_T)
        return a < b;

    assert(false);
    return false;  // remove warning
}

inline bool LexOrder::leq(const Lit a, const Lit b) const {
    assert(a != LIT_UNDEF);
    assert(b != LIT_UNDEF);

    return _order.at(a) < _order.at(b);
}

}  // namespace cosy
#endif  // INCLUDE_DSB_LEXORDER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
