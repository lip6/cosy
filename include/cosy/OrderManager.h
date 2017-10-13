/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_ORDERMANAGER_H_
#define INCLUDE_DSB_ORDERMANAGER_H_

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include "cosy/Types.h"

namespace cosy {

enum OrderType {
    UNKNOWN,
    INCREASING,
    DECREASING,
    OCCURENCE,
    CLASS_INCREASING,
    CLASS_OCCURENCE,
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

        void generate(OrderType type);
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
};

inline void OrderManager::generate(OrderType type) {
    switch (type) {
    case INCREASING: return increase_order();
    case DECREASING: return decrease_order();
    case OCCURENCE:  return occurence_order();
    case CLASS_INCREASING: return class_increase_order();
    case CLASS_OCCURENCE: return class_occurence_order();
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
        // TODO(hakan): maybe test also with sum of occurences in orbit
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

}  // namespace cosy

#endif  // INCLUDE_DSB_ORDERMANAGER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
