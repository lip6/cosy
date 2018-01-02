/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_LITERALADAPTER_H_
#define INCLUDE_DSB_LITERALADAPTER_H_

#include "cosy/Literal.h"

namespace cosy {

template<class T>
class LiteralAdapter {
 public:
        LiteralAdapter() {}
        virtual ~LiteralAdapter() {}

        virtual T convertFrom(cosy::Literal l) = 0;
        virtual cosy::Literal convertTo(T from) = 0;
};

}  // namespace cosy

#endif  // INCLUDE_DSB_LITERALADAPTER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
