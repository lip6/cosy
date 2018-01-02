#include "cosy/PermutationGroup.h"

namespace cosy {

PermutationGroup::PermutationGroup() {
}

PermutationGroup::~PermutationGroup() {
}


void PermutationGroup::addPermutation(std::unique_ptr<Permutation>&& perm) {
    CHECK_NOTNULL(perm);

    const unsigned int permutation_index = _permutations.size();
    const unsigned int num_cycles = perm->numberOfCycles();
    Literal element;

    if (num_cycles == 0)
        return;

    if (perm->size() > _watchers.size())
        _watchers.resize(perm->size());

    for (unsigned int c = 0; c < num_cycles; c++) {
        element = perm->lastElementInCycle(c);
        for (const Literal& image : perm->cycle(c)) {
            _watchers[element.variable()].insert(permutation_index);
            _symmetric_vars.insert(image.variable());

            if (element == image.negated())
                _inverting_vars.insert(element.variable());

            element = image;
        }
    }

    _permutations.emplace_back(perm.release());
}


PermutationGroup::Iterator
PermutationGroup::permutationWith(BooleanVariable variable) const {
    return Iterator(_watchers[variable].begin(),
                    _watchers[variable].end());
}




}  // namespace cosy
