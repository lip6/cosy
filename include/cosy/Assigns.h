/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_ASSIGNS_H_
#define INCLUDE_DSB_ASSIGNS_H_

#include <vector>

#include "cosy/Types.h"

namespace cosy {
class Assigns {
 public:
        explicit Assigns(unsigned int num_vars);
        virtual ~Assigns();

        void setLitTrue(Lit lit, unsigned int level);
        void setLitUndef(Lit lit);

        LiteralValue valueOf(Lit lit) const;
        LiteralValue valueOf(Var var) const;

        int levelOf(Lit lit) const;
        int levelOf(Var var) const;

 private:
        unsigned int _num_vars;
        std::vector<LiteralValue> _values;
        std::vector<int> _levels;
};

inline Assigns::Assigns(unsigned int num_vars) : _num_vars(num_vars) {
    _values.assign(num_vars + 1, LiteralUndef);
    _levels.assign(num_vars + 1, LEVEL_UNDEF);
}

inline Assigns::~Assigns() {
}

inline void Assigns::setLitTrue(Lit lit, unsigned int level) {
    _values[varOf(lit)] = createLiteralValue(!sign(lit));
    _levels[varOf(lit)] = level;
}

inline void Assigns::setLitUndef(Lit lit) {
    _values[varOf(lit)] = LiteralUndef;
    _levels[varOf(lit)] = LEVEL_UNDEF;
}

inline LiteralValue Assigns::valueOf(Lit lit) const {
    Var var = varOf(lit);

    if (!sign(lit))
        return _values[var];
    if (_values[var] == LiteralUndef)
        return LiteralUndef;
    if (_values[var] == LiteralTrue)
        return LiteralFalse;
    return LiteralTrue;
}

inline LiteralValue Assigns::valueOf(Var var) const {
    return _values[var];
}

inline int Assigns::levelOf(Lit lit) const {
    return _levels[varOf(lit)];
}

inline int Assigns::levelOf(Var var) const {
    return _levels[var];
}

}  // namespace cosy
#endif  // INCLUDE_DSB_ASSIGNS_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
