#ifndef INCLUDE_COSY_PERMUTATIONGROUP_H_
#define INCLUDE_COSY_PERMUTATIONGROUP_H_


#include <unordered_set>
#include <memory>
#include <vector>

#include "cosy/DisjointSets.h"
#include "cosy/int_type_indexed_vector.h"
#include "cosy/macros.h"
#include "cosy/Permutation.h"

namespace cosy {

class PermutationGroup {
 public:
    PermutationGroup();
    ~PermutationGroup();

    void addPermutation(std::unique_ptr<Permutation>&& perm);

    struct Iterator;
    Iterator permutationWith(BooleanVariable variable) const;

    std::vector<std::unique_ptr<Permutation>>::const_iterator begin() const  {
        return _permutations.begin();
    }
    std::vector<std::unique_ptr<Permutation>>::const_iterator end() const {
        return _permutations.end();
    }

    const std::vector<std::unique_ptr<Permutation>>& permutations() const {
        return _permutations;
    }

    const std::unordered_set<BooleanVariable>& symmetrics() const {
        return _symmetric_vars;
    }


    uint64_t numberOfSymmetricVars() const { return _symmetric_vars.size(); }
    uint64_t numberOfInvertingVars() const { return _inverting_vars.size(); }
    uint64_t numberOfPermutations() const { return _permutations.size(); }

 private:
    std::vector<std::unique_ptr<Permutation>> _permutations;
    std::unordered_set<BooleanVariable> _symmetric_vars;
    std::unordered_set<BooleanVariable> _inverting_vars;

    ITIVector<BooleanVariable, std::unordered_set<unsigned int>> _watchers;

    DISALLOW_COPY_AND_ASSIGN(PermutationGroup);
};


struct PermutationGroup::Iterator {
    typedef unsigned int value_type;
    typedef std::unordered_set<unsigned int>::const_iterator const_iterator;

    Iterator() {}
    Iterator(const const_iterator &b, const const_iterator &e) :
        _begin(b), _end(e) {}

    const_iterator begin() const { return _begin; }
    const_iterator end() const { return _end; }
    const const_iterator _begin;
    const const_iterator _end;

    int size() const { return std::distance(begin(), end()); }
};

}  // namespace cosy

#endif  // INCLUDE_COSY_PERMUTATIONGROUP_H_
