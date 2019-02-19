/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_ORDERMANAGER_H_
#define INCLUDE_DSB_ORDERMANAGER_H_

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>

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
    explicit OrderManager(const unsigned int num_vars) :
        _num_vars(num_vars),
        _type(UNKNOWN) {}
    ~OrderManager() {}

    void generate(OrderType type,
                  const std::vector<std::unique_ptr<Permutation>>& G);
    void custom(const std::vector<Lit>& order) {
        _type = CUSTOM;
        _order = order;
    }

    const std::vector<Lit>& order() const { return _order; }
    const std::string orderString() const;

    // We make a copy one time it's acceptable
    std::vector< std::vector<Var> >&orbits() { return _orbits; }
    std::vector<int>& occurences() { return _occurences; }

 private:
    const unsigned int _num_vars;
    OrderType _type;
    std::vector<Lit> _order;
    std::vector< std::vector<Var> > _orbits;
    std::vector<int> _occurences;

    void increase_order();
    void decrease_order();
    void occurence_order();
    void class_increase_order();
    void class_occurence_order();
    void breakid_order(const std::vector<std::unique_ptr<Permutation>>& G);
};

inline void
OrderManager::generate(OrderType type,
                       const std::vector<std::unique_ptr<Permutation>>& G) {
    switch (type) {
    case INCREASING: return increase_order();
    case DECREASING: return decrease_order();
    case OCCURENCE:  return occurence_order();
    case CLASS_INCREASING: return class_increase_order();
    case CLASS_OCCURENCE: return class_occurence_order();
    case BREAKID: return breakid_order(G);
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
    case CUSTOM:           return std::string("custom");
    case BREAKID:           return std::string("breakid");

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

    std::vector<int> indexes;
    indexes.assign(_num_vars + 1, 0);

    for (unsigned int i = 1; i <= _num_vars; i++)
        indexes[i] = i;
    std::sort(indexes.begin() + 1, indexes.end(),
              ValueOrderLt<int>(_occurences));

    for (unsigned int i = 1; i <= _num_vars; i++)
        _order.push_back(indexes[i]);
    for (unsigned int i = 1; i <= _num_vars; i++)
        _order.push_back(negate(indexes[i]));
}

inline void OrderManager::class_increase_order() {
    _type = CLASS_INCREASING;

    for (const std::vector<Var>& orbit : _orbits)
        for (const Var var : orbit)
            _order.push_back(var);

    for (const std::vector<Var>& orbit : _orbits)
        for (const Var var : orbit)
            _order.push_back(negate(var));

    // TODO(hakan): Maybe we can also sort the orbit's order
}

inline void OrderManager::class_occurence_order() {
    _type = CLASS_OCCURENCE;

    std::vector< std::vector<Var> > orbits = _orbits;
    std::vector<int> orbits_order;
    std::vector<int> orbits_occurence;


    for (std::vector<Var>& orbit : orbits)
        std::sort(orbit.begin(), orbit.end(), ValueOrderLt<int>(_occurences));

    for (unsigned int i = 0; i < orbits.size(); i++) {
        orbits_order.push_back(i);
        // Sort with the highest occurence of each orbit
        // TODO(user): maybe test also with sum of occurences in orbit
        orbits_occurence.push_back(_occurences[orbits[i][0]]);
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


inline void
OrderManager::breakid_order(const std::vector<std::unique_ptr<Permutation>>& G) {
    _type = BREAKID;

    std::vector<Permutation*> Q, filter;
    std::unordered_map<Var, int> occurence_generators;
    unsigned int largestOrbit;
    Var next = VAR_UNDEF;
    Orbits orbits;

    std::vector<Var> ord;

    for (const std::unique_ptr<Permutation>& perm : G)
        Q.push_back(perm.get());

    while (Q.size() > 0) {
        // Compute occurence in remain generators
        occurence_generators.clear();
        for (const Permutation* permutation : Q) {
            for (const Lit& literal : permutation->support())
                if (!sign(literal))
                    occurence_generators[varOf(literal)]++;
        }
        occurence_generators[VAR_UNDEF] = std::numeric_limits<int>::max();

        orbits.compute(Q);

        largestOrbit = 0;
        next = VAR_UNDEF;

        for (const std::vector<Var>& orbit : orbits.orbits()) {
            if (orbit.size() < largestOrbit)
                continue;
            for (const Var variable : orbit) {
                if (occurence_generators[variable] == 0)
                    continue;
                const int occ_v = occurence_generators[variable];
                const int occ_n = occurence_generators[next];
                if (next == VAR_UNDEF ||
                    occ_v < occ_n || (occ_v == occ_n && variable < next)) {
                    next = variable;
                    largestOrbit = orbit.size();
                }
            }
        }
        if (next == VAR_UNDEF) {
            std::cout << "Problem with generators ??";
            break;
        }

        ord.push_back(next);

        filter.clear();
        const Lit next_literal = createLiteral(next, false);
        for (Permutation* p : Q) {
            if (std::find(std::begin(p->support()),
                          std::end(p->support()),
                          next_literal) == std::end(p->support()))
                filter.push_back(p);
        }

        Q = filter;
    }


    for (int i = 1; i <= static_cast<int>(_num_vars); i++)
        if (std::find(std::begin(ord), std::end(ord), i) == std::end(ord))
            ord.push_back(i);

    for (Var v : ord)
        _order.push_back(v);
    for (Var v : ord)
        _order.push_back(negate(v));
}

}  // namespace cosy

#endif  // INCLUDE_DSB_ORDERMANAGER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
