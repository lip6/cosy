#ifndef INCLUDE_COSY_PERMUTATIONINFO_H_
#define INCLUDE_COSY_PERMUTATIONINFO_H_

#include <unordered_map>
#include <vector>

#include "cosy/Literal.h"
#include "cosy/Permutation.h"


namespace cosy {

class PermutationInfo {
 public:
    explicit PermutationInfo(const unsigned int permutation_index) :
    _permutation_index(permutation_index) {
    }

    ~PermutationInfo() {
    }

    void registerInverse(const Literal& e, const Literal& i) {
        _inverse[i] = e;
    }

    Literal inverseOf(const Literal& e) const { return _inverse.at(e); }

 private:
    const unsigned int _permutation_index;

    std::unordered_map<Literal, Literal> _inverse;
};

}  // namespace cosy

#endif  // INCLUDE_COSY_PERMUTATIONINFO_H_
