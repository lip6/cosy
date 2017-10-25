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

        void compute(const std::vector<std::unique_ptr<Permutation>>& perms);
        void compute(const std::vector<Permutation*>& perms);

        void debugPrint() const;

        const std::vector<Var> largestOrbit() const;
        std::vector< std::vector<Var> > orbits() const;

        uint64_t numOrbits() const  { return _orbits.size(); }
 private:
        std::unordered_map<Lit, std::vector<Var>> _orbits;
        std::unordered_set<Var> _symmetrics;
};

inline void
Orbits::compute(const std::vector<std::unique_ptr<Permutation>>& perms) {
    DisjointSets disjoint_sets;

    _symmetrics.clear();
    _orbits.clear();

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


inline void Orbits::compute(const std::vector<Permutation*>& perms) {
    DisjointSets disjoint_sets;

    _symmetrics.clear();
    _orbits.clear();

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



inline void Orbits::debugPrint() const {
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


inline const std::vector<Var> Orbits::largestOrbit() const {
    std::vector< std::vector<Var> > sorted_orbits = orbits();
    unsigned int max = 0;

    if (sorted_orbits.empty())
        return std::vector<Var>();

    for (unsigned int i=1; i<sorted_orbits.size(); i++)
        if (sorted_orbits[i].size() > sorted_orbits[max].size())
            max = i;

    return sorted_orbits[max];

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
