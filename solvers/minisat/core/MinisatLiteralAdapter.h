#ifndef MinisatLiteralAdapter_h
#define MinisatLiteralAdapter_h

#include "cosy/LiteralAdapter.h"
#include "core/SolverTypes.h"
#include "cosy/Literal.h"

#include <iostream>

namespace Minisat {

class MinisatLiteralAdapter : public cosy::LiteralAdapter<Minisat::Lit>
{
 public:
        MinisatLiteralAdapter() {}
        ~MinisatLiteralAdapter() {}

        virtual Minisat::Lit convertFrom(cosy::Literal l) {
            return mkLit(l.variable().value(), l.isNegative());
        }

        virtual cosy::Literal convertTo(Minisat::Lit lit) {
            return (sign(lit) ? -(var(lit) + 1) : var(lit) + 1);
        }
};

}

#endif
