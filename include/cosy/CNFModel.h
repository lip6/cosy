// Copyright 2017 Hakan Metin
// Licensed under the GPL, Version 3.0 (the "License");


#ifndef SAT_CORE_CNFMODEL_H_
#define SAT_CORE_CNFMODEL_H_

#include <algorithm>
#include <memory>
#include <vector>

#include "cosy/int_type_indexed_vector.h"
#include "cosy/Clause.h"
#include "cosy/Literal.h"
#include "cosy/logging.h"

namespace cosy {

class CNFModel {
 public:
    CNFModel();
    ~CNFModel();

    void addClause(std::vector<Literal>& literals);

    uint64_t numberOfVariables() const;
    uint64_t numberOfClauses() const;

    uint64_t numberOfUnaryClauses()   const { return _unary_clauses.size();   }
    uint64_t numberOfBinaryClauses()  const { return _binary_clauses.size();  }
    uint64_t numberOfTernaryClauses() const { return _ternary_clauses.size(); }
    uint64_t numberOfOtherClauses()   const { return _clauses.size();         }

    const std::vector<std::unique_ptr<Clause>>& unaryClauses() const {
        return _unary_clauses;
    }

    const std::vector<std::unique_ptr<Clause>>& binaryClauses() const{
        return _binary_clauses;
    }

    const std::vector<std::unique_ptr<Clause>>& ternaryClauses() const {
        return _ternary_clauses;
    }

    const std::vector<std::unique_ptr<Clause>>& otherClauses() const {
        return _clauses;
    }

    const ITIVector<BooleanVariable, unsigned int>& occurences() const {
        return _occurences;
    }

 private:
    uint64_t _num_variables;
    std::vector<std::unique_ptr<Clause>> _unary_clauses;
    std::vector<std::unique_ptr<Clause>> _binary_clauses;
    std::vector<std::unique_ptr<Clause>> _ternary_clauses;
    std::vector<std::unique_ptr<Clause>> _clauses;

    ITIVector<BooleanVariable, unsigned int> _occurences;

    DISALLOW_COPY_AND_ASSIGN(CNFModel);
};

}  // namespace cosy

#endif  // SAT_CORE_CNFMODEL_H_
