/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_PARSERSAUCY_H_
#define INCLUDE_DSB_PARSERSAUCY_H_

#include <memory>
#include <string>
#include <vector>

#include "cosy/Parser.h"
#include "cosy/StreamBuffer.h"
#include "cosy/Permutation.h"
#include "cosy/Types.h"

namespace cosy {
class ParserSaucy : public Parser {
 public:
        ParserSaucy();
        virtual ~ParserSaucy();

        std::vector< std::unique_ptr<Permutation> >
        parse(unsigned int num_vars, const std::string symmetryFile);
 private:
};

}  // namespace cosy
#endif  // INCLUDE_DSB_PARSERSAUCY_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
