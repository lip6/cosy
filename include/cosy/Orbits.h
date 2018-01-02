/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_COSY_ORBITS_H_
#define INCLUDE_COSY_ORBITS_H_

#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <algorithm>

#include "cosy/int_type_indexed_vector.h"
#include "cosy/Literal.h"
#include "cosy/DisjointSets.h"
#include "cosy/Permutation.h"

namespace cosy {

class Orbits {
 public:
    Orbits();
    explicit Orbits(const std::vector<std::unique_ptr<Permutation>>& perms);
    explicit Orbits(const std::vector<Permutation*> perms);

    ~Orbits();

    void assign(const std::vector<std::unique_ptr<Permutation>>& perms);
    void assign(const std::vector<Permutation*> perms);
    void clear();

    const std::vector<ITIVector<BooleanVariable, BooleanVariable>>&
    orbits() const { return _orbits; }

    uint64_t numberOfOrbits() const { return _orbits.size(); }

    void debugPrint();

 private:
    std::vector< ITIVector<BooleanVariable, BooleanVariable> > _orbits;
};


}  // namespace cosy
#endif  // INCLUDE_COSY_ORBITS_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
