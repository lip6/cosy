#ifndef GlucoseLiteralAdapter_h
#define GlucoseLiteralAdapter_h

#include <iostream>

#include "core/SolverTypes.h"

#include "cosy/LiteralAdapter.h"
#include "cosy/Types.h"


namespace Glucose {

class GlucoseLiteralAdapter : public cosy::LiteralAdapter<Glucose::Lit>
{
 public:
        GlucoseLiteralAdapter() {}
        ~GlucoseLiteralAdapter() {}

        virtual Glucose::Lit convert(cosy::Lit l) {
            return mkLit(cosy::varOf(l) - 1, cosy::sign(l));
        }

        virtual cosy::Lit convert(Glucose::Lit lit) {
            return (sign(lit) ? -(var(lit) + 1) : var(lit) + 1);
        }
};

}

#endif
