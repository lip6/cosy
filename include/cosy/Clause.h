// Copyright 2017 Hakan Metin
// Licensed under the Apache License, Version 2.0 (the "License");

#ifndef SAT_CORE_CLAUSE_H_
#define SAT_CORE_CLAUSE_H_

#include <string>
#include <vector>

#include "cosy/Literal.h"
#include "cosy/logging.h"

namespace cosy {

class Clause {
 public:
    Clause(const std::vector<Literal>& literals) : _literals(literals) {}
    ~Clause() {}

    // Allows for range based iteration: for (Literal literal : clause) {}.
    std::vector<Literal>::const_iterator begin() const {
        return _literals.begin();
    }
    std::vector<Literal>::const_iterator end() const { return _literals.end(); }

    int  size()         const { return _literals.size();        }

 private:
    std::vector<Literal> _literals;

    DISALLOW_COPY_AND_ASSIGN(Clause);
};

}  // namespace cosy

#endif  // SAT_CORE_CLAUSE_H_
