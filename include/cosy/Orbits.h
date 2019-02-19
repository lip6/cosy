/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_ORBITS_H_
#define INCLUDE_DSB_ORBITS_H_

#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <algorithm>

#include "cosy/DisjointSets.h"
#include "cosy/Permutation.h"
#include "cosy/Types.h"

namespace cosy {

class Orbits {
 public:
        Orbits() {}
        ~Orbits() {}

        void compute(const std::vector<Permutation*>& perms);

        void compute(const std::vector<std::unique_ptr<Permutation>>& perms);
        uint64_t numOrbits() const;
        void printOrbits() const;

        std::vector< std::vector<Var> > orbits() const;

 private:
        std::unordered_map<Lit, std::vector<Var>> _orbits;
        std::unordered_set<Var> _symmetrics;
};

inline void
Orbits::compute(const std::vector<std::unique_ptr<Permutation>>& perms) {
    DisjointSets disjoint_sets;

    _orbits.clear();
    _symmetrics.clear();

    for (const std::unique_ptr<Permutation>& permutation : perms) {
        for (int c = 0; c < permutation->numCycles(); ++c) {
            Lit element = permutation->lastElementInCycle(c);
            Var v_element = varOf(element);

            disjoint_sets.Add(v_element);
            for (const Lit image : permutation->cycle(c)) {
                Var v_image = varOf(image);

                _symmetrics.insert(v_element);

                disjoint_sets.Add(v_image);
                disjoint_sets.Union(v_element, v_image);
                element = image;
                v_element = varOf(element);
            }
        }
    }

    for (const Var& var : _symmetrics) {
        const Var representant = disjoint_sets.Find(var);
        _orbits[representant].push_back(var);
    }
}

inline void
Orbits::compute(const std::vector<Permutation*>& perms) {
    DisjointSets disjoint_sets;

    _orbits.clear();
    _symmetrics.clear();

    for (const Permutation* permutation : perms) {
        for (int c = 0; c < permutation->numCycles(); ++c) {
            Lit element = permutation->lastElementInCycle(c);
            Var v_element = varOf(element);

            disjoint_sets.Add(v_element);
            for (const Lit image : permutation->cycle(c)) {
                Var v_image = varOf(image);

                _symmetrics.insert(v_element);

                disjoint_sets.Add(v_image);
                disjoint_sets.Union(v_element, v_image);
                element = image;
                v_element = varOf(element);
            }
        }
    }

    for (const Var& var : _symmetrics) {
        const Var representant = disjoint_sets.Find(var);
        _orbits[representant].push_back(var);
    }
}



inline uint64_t Orbits::numOrbits() const { return _orbits.size(); }

inline void Orbits::printOrbits() const {
    unsigned int i = 1;
    std::vector<Var> orbit;

    for (const std::pair<Var, std::vector<Var>> p : _orbits) {
        std ::cout << "[" << i << "]: ";

        orbit = p.second;
        std::sort(orbit.begin(), orbit.end());

        for (const Var& var : orbit) {
            std::cout << var << " ";
        }
        std::cout << std::endl;
        i++;
    }
}


inline std::vector< std::vector<Var> > Orbits::orbits() const {
    std::vector< std::vector<Var> > sorted_orbits;
    std::vector<Var> orbit;

    for (const std::pair<Var, std::vector<Var>>& p : _orbits) {
        orbit = p.second;
        std::sort(orbit.begin(), orbit.end());
        sorted_orbits.push_back(orbit);
    }

    return sorted_orbits;
}

}  // namespace cosy
#endif  // INCLUDE_DSB_ORBITS_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
