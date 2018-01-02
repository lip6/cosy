#ifndef INCLUDE_COSY_IDENTITYLITERALADPATER_H_
#define INCLUDE_COSY_IDENTITYLITERALADPATER_H_

#include "cosy/LiteralAdapter.h"
#include "cosy/Literal.h"

#include <iostream>

namespace cosy {

class IdentityLiteralAdapter : public cosy::LiteralAdapter<cosy::Literal> {
 public:
        IdentityLiteralAdapter() {}
        ~IdentityLiteralAdapter() {}

        virtual cosy::Literal convertTo(cosy::Literal lit) {
            return lit;
        }

        virtual cosy::Literal convertFrom(cosy::Literal lit) {
            return lit;
        }
};

}

#endif
