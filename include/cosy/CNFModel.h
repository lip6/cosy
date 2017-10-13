/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_CNFMODEL_H_
#define INCLUDE_DSB_CNFMODEL_H_

#include <cassert>
#include <vector>

#include "cosy/Types.h"

namespace cosy {

class CNFModel {
 public:
        CNFModel() : _num_vars(0),
                     _closed(true),
                     _num_unary(0),
                     _num_binary(0),
                     _num_ternary(0) {}
        ~CNFModel() {}

        void addToCurrentClause(Lit lit);
        void closeCurrentClause();

        uint64_t numVars()    const { return _num_vars; }
        uint64_t numClauses() const { return _clauses.size(); }
        uint64_t numUnary()      const { return _num_unary; }
        uint64_t numBinary()     const { return _num_binary; }
        uint64_t numTernary()    const { return _num_ternary; }
        const std::vector<int>& occurences() const { return _occurences; }

        void printStats();

        void debugPrint();


 private:
        unsigned int _num_vars;
        std::vector<Clause> _clauses;
        bool _closed;

        std::vector<int> _occurences;
        uint64_t _num_unary;
        uint64_t _num_binary;
        uint64_t _num_ternary;
};

inline void CNFModel::addToCurrentClause(Lit lit) {
    if (varOf(lit) > _num_vars)
        _num_vars = varOf(lit);

    if (_closed) {
        _clauses.push_back(Clause());
        _closed = false;
    }
    _clauses.back().push_back(lit);

    if (_num_vars >= _occurences.size())
        _occurences.resize(_num_vars + 1, 0);
    _occurences[varOf(lit)]++;
}

inline void CNFModel::closeCurrentClause() {
    assert(_clauses.back().size() > 0);

    _closed = true;

    switch (_clauses.back().size()) {
    case 1: _num_unary++;   break;
    case 2: _num_binary++;  break;
    case 3: _num_ternary++; break;
    }
}

}  // namespace cosy

#endif  // INCLUDE_DSB_CNFMODEL_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
