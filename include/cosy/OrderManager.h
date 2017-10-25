/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_ORDERMANAGER_H_
#define INCLUDE_DSB_ORDERMANAGER_H_

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include <map>

#include "cosy/Types.h"
#include "cosy/Permutation.h"
#include "cosy/Orbits.h"

namespace cosy {

enum OrderType {
    UNKNOWN,
    INCREASING,
    DECREASING,
    OCCURENCE,
    CLASS_INCREASING,
    CLASS_OCCURENCE,
    BREAKID,
    AUTO,
    CUSTOM
};

template<typename T>
struct ValueOrderLt {
        const std::vector<T>& values;
        bool operator() (unsigned int i, unsigned int j) {
            if (values[i] == values[j])
                return i < j;
            return values[i] > values[j];
        }
        explicit ValueOrderLt(const std::vector<T>& v) : values(v) {}
};

class OrderManager {
 public:
        OrderManager(const unsigned int num_vars,
                     const std::vector<int>& occurences,
                     const std::vector<Permutation*>& permutations,
                     unsigned int inverting) :
            _num_vars(num_vars),
            _type(UNKNOWN),
            _occurences(occurences),
            _permutations(permutations),
            _inverting(inverting) {}

        ~OrderManager() {}

        void generate(OrderType type);
        void custom(const std::vector<Lit>& order) {
            _type = CUSTOM;
            _order = order;
        }

        const std::vector<Lit>& order() const { return _order; }
        const std::string orderString() const;

 private:
        const unsigned int _num_vars;
        OrderType _type;
        std::vector<Lit> _order;

        std::vector<int> _occurences;
        std::vector<Permutation *> _permutations;
        unsigned int _inverting;

        void increase_order();
        void decrease_order();
        void occurence_order();
        void class_increase_order();
        void class_occurence_order();
        void breakid_order();
        void auto_order();
};

inline void OrderManager::generate(OrderType type) {
    switch (type) {
    case INCREASING: return increase_order();
    case DECREASING: return decrease_order();
    case OCCURENCE:  return occurence_order();
    case CLASS_INCREASING: return class_increase_order();
    case CLASS_OCCURENCE: return class_occurence_order();
    case BREAKID: return breakid_order();
    case AUTO: return auto_order();

    default:
        assert(false);
    }
}

inline const std::string OrderManager::orderString() const {
    switch (_type) {
    case UNKNOWN:          return std::string("unknown");
    case INCREASING:       return std::string("increase");
    case DECREASING:       return std::string("decrease");
    case OCCURENCE:        return std::string("occurence");
    case CLASS_INCREASING: return std::string("class increase");
    case CLASS_OCCURENCE:  return std::string("class occurence");
    case BREAKID:          return std::string("breakid");
    case AUTO:             return std::string("auto");
    case CUSTOM:           return std::string("custom");
    default: assert(false);
    }
    return "";
}

inline void OrderManager::increase_order() {
    _type = INCREASING;

    for (int i = 1; i <= static_cast<int>(_num_vars); i++)
        _order.push_back(i);
    for (int i = 1; i <= static_cast<int>(_num_vars); i++)
        _order.push_back(negate(i));
}

inline void OrderManager::decrease_order() {
    _type = DECREASING;

    for (int i = static_cast<int>(_num_vars); i > 0; i--)
        _order.push_back(i);
    for (int i = static_cast<int>(_num_vars); i > 0; i--)
        _order.push_back(negate(i));
}

inline void OrderManager::occurence_order() {
    _type = OCCURENCE;

    std::vector<int> occurences = _occurences;
    std::vector<int> indexes;
    indexes.assign(_num_vars + 1, 0);

    for (unsigned int i = 1; i <= _num_vars; i++)
        indexes[i] = i;
    std::sort(indexes.begin() + 1, indexes.end(),
              ValueOrderLt<int>(occurences));

    for (unsigned int i = 1; i <= _num_vars; i++)
        _order.push_back(indexes[i]);
    for (unsigned int i = 1; i <= _num_vars; i++)
        _order.push_back(negate(indexes[i]));
}

inline void OrderManager::class_increase_order() {
    _type = CLASS_INCREASING;

    Orbits orbits;
    orbits.compute(_permutations);

    for (const std::vector<Var>& orbit : orbits.orbits())
        for (const Var var : orbit)
            _order.push_back(var);

    for (const std::vector<Var>& orbit : orbits.orbits())
        for (const Var var : orbit)
            _order.push_back(negate(var));

    // TODO(user): Maybe we can also sort the orbit's order
}

inline void OrderManager::class_occurence_order() {
    _type = CLASS_OCCURENCE;

    std::vector<int> orbits_order;
    std::vector<int> orbits_occurence;
    std::vector<int> occurences = _occurences;

    Orbits O;
    O.compute(_permutations);
    std::vector< std::vector<Var> > orbits = O.orbits();


    for (std::vector<Var>& orbit : orbits)
        std::sort(orbit.begin(), orbit.end(), ValueOrderLt<int>(occurences));

    for (unsigned int i = 0; i < orbits.size(); i++) {
        orbits_order.push_back(i);
        // Sort with the highest occurence of each orbit
        // TODO(user): maybe test also with sum of occurences in orbit
        orbits_occurence.push_back(occurences[orbits[i][0]]);
    }
    std::sort(orbits_order.begin(), orbits_order.end(),
              ValueOrderLt<int>(orbits_occurence));

    for (int index : orbits_order) {
        const std::vector<Var>& orbit = orbits[index];
        for (const Var& var : orbit)
            _order.push_back(var);
    }
    for (int index : orbits_order) {
        const std::vector<Var>& orbit = orbits[index];
        for (const Var& var : orbit)
            _order.push_back(negate(var));
    }
}

inline void OrderManager::breakid_order() {
    Orbits orbits;
    std::vector<Var> order;

    std::vector<Permutation*> Q(_permutations);
    std::vector<Permutation*> filter;

    std::map<Var, int> occurences;  // number of occurence in Q

    Var next = VAR_UNDEF;
    unsigned int largestOrbitSize;

    _type = BREAKID;

    // for (Permutation* p : Q)
    //     p->debugPrint();

    while (Q.size() > 0) {
        occurences.clear();
        /* Computes occurences on Q */
        for (const Permutation* permutation : Q)
            for (const Lit& lit : permutation->support())
                if (!sign(lit))
                    occurences[lit]++;

        /* Compute Orbits */
        orbits.compute(Q);

        largestOrbitSize = 0;
        next = VAR_UNDEF;

        for (const std::vector<Var>& orbit : orbits.orbits()) {
            if (next == VAR_UNDEF) {
                largestOrbitSize = orbit.size();
                for (const Var& var : orbit)
                    if (next == VAR_UNDEF ||
                        occurences[var] < occurences[next] ||
                        (occurences[var] == occurences[next] && var < next))
                        next = var;
            } else if (orbit.size() >= largestOrbitSize) {
                for (const Var& var : orbit)
                    if (occurences[var] < occurences[next] ||
                        (occurences[var] == occurences[next] && var < next)) {
                        next = var;
                        largestOrbitSize = orbit.size();
                    }
            }
        }

        order.push_back(next);

        filter.clear();
        for (Permutation* p : Q) {
            if (!p->contains(next))
                filter.push_back(p);
        }
        Q = filter;
    }

    /* Complete order */
    std::unordered_set<Var> in(order.begin(), order.end());

    /* Fill lex order */
    for (unsigned int i=1; i<=_num_vars; i++)
        if (in.find(i) == in.end())
            order.push_back(i);

    for (const Var var : order)
        _order.push_back(var);
    for (Var var : order)
        _order.push_back(negate(var));

}


inline void OrderManager::auto_order() {
    if ((static_cast<double>(_inverting) / _num_vars) > 0.25)
        breakid_order();
    else
        occurence_order();
}

}  // namespace cosy

#endif  // INCLUDE_DSB_ORDERMANAGER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
