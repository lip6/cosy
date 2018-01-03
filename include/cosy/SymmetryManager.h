// Copyright 2017 Hakan Metin


#ifndef INCLUDE_COSY_SYMMETRYMANAGER_H_
#define INCLUDE_COSY_SYMMETRYMANAGER_H_

#include <iostream>
#include <vector>
#include <unordered_set>
#include <memory>

#include "cosy/Assignment.h"
#include "cosy/LexLeader.h"
#include "cosy/LexLeaderForcing.h"
#include "cosy/Literal.h"
#include "cosy/Order.h"
#include "cosy/PermutationGroup.h"
#include "cosy/PermutationStatus.h"
#include "cosy/PermutationSPFS.h"
#include "cosy/Stats.h"

namespace cosy {

class SymmetryManager {
 public:
    explicit SymmetryManager(unsigned int num_vars,
                             const PermutationGroup& group);
    ~SymmetryManager();

    void setOrder(std::unique_ptr<Order> && order);

    void updateNotify(const Literal& literal);
    void updateCancel(const Literal& literal);

    bool isNotLexLeader(const Literal& literal) const;
    const std::unique_ptr<Clause>& generateESBP();

    // void searchLexLeaderForcing(Literal literal);

    // bool canForceLexLeader(Literal literal) const;
    // std::unique_ptr<Clause> generateForceLexLeaderESBP();

    bool isUnitsLiteral() const;
    Literal unitLiteral();
    uint64_t numberOfUnits() const { return _units_clauses.size(); }

    void debugInactiveStats() const;
    void debugObservationStats() const;
    void debugESBPStats() const;
    void debugPermutationStatusStats() const;
    void debugFullObservation() const;


    void printStats() const { _stats.print(); }
 private:
    unsigned int _num_vars;
    const PermutationGroup& _group;
    Assignment _assignment;
    LexLeader _lex_leader;
    LexLeaderForcing _lex_leader_forcing;

    std::unique_ptr<Order> _order;
    std::vector<std::unique_ptr<PermutationStatus>> _statuses;
    std::unordered_set<Literal> _units_clauses;

    /* SPFS */
    std::vector<std::unique_ptr<PermutationSPFS>> _symmetries_spfs;


    void updateOrderedOrders(Literal literal);
    void findUnitClauses();

    void tryAddOrder(Literal literal);


    struct Stats : public StatsGroup {
        Stats() : StatsGroup("Symmetry Manager"),
            esbp("Number of ESBP", this),
            esbp_forcing("Number of ESBP forcing", this)

        {}
        CumulativeStat esbp;
        CumulativeStat esbp_forcing;

    };
    Stats _stats;

    DISALLOW_COPY_AND_ASSIGN(SymmetryManager);
};

}  // namespace cosy

#endif  // INCLUDE_COSY_SYMMETRYMANAGER_H_
