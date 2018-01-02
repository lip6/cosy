#ifndef INCLUDE_COSY_PERMUTATIONSTATUS_H_
#define INCLUDE_COSY_PERMUTATIONSTATUS_H_

#include <algorithm>
#include <deque>
#include <unordered_map>
#include <vector>
#include <memory>

#include "cosy/Assignment.h"
#include "cosy/Literal.h"
#include "cosy/LexLeader.h"
#include "cosy/Order.h"
#include "cosy/Stats.h"
#include "cosy/logging.h"
#include "cosy/Permutation.h"


namespace cosy {

enum Status {
    ACTIVE,
    REDUCER,
    INACTIVE,
    PERMANENTLY_INACTIVE
};

class PermutationStatus {
    friend class SymmetryManager;

 public:
    explicit PermutationStatus(const unsigned int permutation_index,
                               const Assignment& assignment,
                               const std::unique_ptr<Order>& order,
                               const std::unique_ptr<Permutation>& permutation);
    ~PermutationStatus();

    void addToOrderedSupport(const Literal& literal);

    bool updateNotifyLookupVariable(Literal literal);
    bool updateCancelLookupVariable(Literal literal);

    // Accessors
    BooleanVariable lookupVariable() const;
    BooleanVariable lookupVariableInverse() const;

    Literal lookupLiteral() const;
    Literal lookupLiteralInverse() const;

    bool isEndOfOrder() const;
    Literal inverseOf(const Literal& literal) const;

    bool isActive()   const;
    bool isReducer()  const;
    bool isInactive() const;

    void reset();
    void generateESBP(BooleanVariable reason, LexLeader *lex);


    // bool canForceLexLeader();

    // std::unique_ptr<Clause>
    // generateForceLexLeaderESBP(BooleanVariable reason);

    void debugPrint() const;
    void observed(std::unordered_set<BooleanVariable> *variables) const;


 private:
    const unsigned int _permutation_index;
    const Assignment& _assignment;
    const std::unique_ptr<Order>& _order;

    Status _status;

    unsigned int _lookup_index;
    std::vector<Literal> _ordered_support;

    struct Lookup {
     Lookup(BooleanVariable v, unsigned int i) :
        var(v), index(i) {}
        BooleanVariable var;
        unsigned int index;
    };

    std::deque<Lookup> _trail_index;

    std::unordered_map<Literal, Literal> _inverse;

    Status updateStatus();

    struct Stats : public StatsGroup {
        Stats() : StatsGroup("status"),
                  index("index", this),
                  esbp("esbp", this),
                  touched("touched", this)
        {}

        IntegerDistribution index;
        CumulativeStat esbp;
        CumulativeStat touched;
    };

    Stats _stats;

    DISALLOW_COPY_AND_ASSIGN(PermutationStatus);
};

}  // namespace cosy

#endif  // INCLUDE_COSY_PERMUTATIONSTATUS_H_
