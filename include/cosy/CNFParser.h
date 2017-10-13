/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_CNFPARSER_H_
#define INCLUDE_DSB_CNFPARSER_H_

#include <iostream>
#include <string>

#include "cosy/CNFModel.h"
#include "cosy/StreamBuffer.h"
#include "cosy/Types.h"

namespace cosy {

class CNFParser {
 public:
        CNFParser() {}
        ~CNFParser() {}

        CNFModel parse(const std::string cnf_file);
 private:
};

}  // namespace cosy

#endif  // INCLUDE_DSB_CNFPARSER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
