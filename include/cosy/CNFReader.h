// Copyright 2017 Hakan Metin
// Licensed under the GPL, Version 3.0 (the "License");

#ifndef SAT_CORE_CNFREADER_H_
#define SAT_CORE_CNFREADER_H_

#include <vector>
#include <string>

#include "cosy/CNFModel.h"
#include "cosy/Literal.h"
#include "cosy/StreamBuffer.h"
#include "cosy/logging.h"

namespace cosy {

class CNFReader {
 public:
    CNFReader();
    ~CNFReader();

    bool load(const std::string &filename, CNFModel *model);

 private:
    DISALLOW_COPY_AND_ASSIGN(CNFReader);
};
}  // namespace cosy

#endif  // SAT_CORE_CNFREADER_H_
