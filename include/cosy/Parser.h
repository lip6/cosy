/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_PARSER_H_
#define INCLUDE_DSB_PARSER_H_

#include <vector>
#include <string>
#include <memory>

#include "cosy/Permutation.h"
#include "cosy/Types.h"

namespace cosy {


class Parser {
 public:
        Parser() { }
        virtual ~Parser() { }

        virtual std::vector< std::unique_ptr<Permutation> >
        parse(unsigned int num_vars, const std::string symmetryFile) = 0;

 protected:
};

static inline Lit graphNodeToLit(int node, int num_vars) {
    if (node <= num_vars)
        return node;
    if (node <= 2*num_vars)
        return num_vars - node;
    else
        return LIT_UNDEF;
}

}  // namespace cosy
#endif  // INCLUDE_DSB_PARSER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
