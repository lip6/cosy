#ifndef MinisatLiteralAdapter_h
#define MinisatLiteralAdapter_h

#include "cosy/LiteralAdapter.h"
#include "core/SolverTypes.h"
#include "cosy/Types.h"

#include <iostream>

namespace Minisat {

class MinisatLiteralAdapter : public cosy::LiteralAdapter<Minisat::Lit>
{
 public:
        MinisatLiteralAdapter() {}
        ~MinisatLiteralAdapter() {}

        virtual Minisat::Lit convert(cosy::Lit l) {
            return mkLit(cosy::varOf(l) - 1, cosy::sign(l));
        }

        virtual cosy::Lit convert(Minisat::Lit lit) {
            return (sign(lit) ? -(var(lit) + 1) : var(lit) + 1);
        }
};

}

#endif
