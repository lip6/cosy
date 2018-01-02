// Copyright 2017 Hakan Metin

#include "cosy/CNFModel.h"

namespace cosy {

CNFModel::CNFModel() : _num_variables(0) {
}

CNFModel::~CNFModel() {
}

void CNFModel::addClause(std::vector<Literal>& literals) {
    CHECK_GT(literals.size(), static_cast<unsigned int>(0));

    /* Remove duplicate literals in clause */
    std::sort(literals.begin(), literals.end());
    auto last_element_it = std::unique(literals.begin(), literals.end());
    literals.erase(last_element_it, literals.end());

    BooleanVariable var = literals.back().variable();
    if (var > _num_variables)
        _num_variables = var.value();

    /* Occurence stats */
    if (numberOfVariables() > _occurences.size())
        _occurences.resize(numberOfVariables());
    for (const Literal& lit : literals)
        _occurences[lit.variable()]++;

    /* Register clause */
    std::unique_ptr<Clause> clause(new Clause(literals));
    switch (clause->size()) {
    case 1:  _unary_clauses.emplace_back(clause.release());   break;
    case 2:  _binary_clauses.emplace_back(clause.release());  break;
    case 3:  _ternary_clauses.emplace_back(clause.release()); break;
    default: _clauses.emplace_back(clause.release());         break;
    }

}

uint64_t CNFModel::numberOfVariables() const {
    return _num_variables + 1; // Because start from 0
}
uint64_t CNFModel::numberOfClauses() const {
    return _unary_clauses.size() + _binary_clauses.size() +
        _ternary_clauses.size() +_clauses.size();
}

}  // namespace cosy
