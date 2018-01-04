#ifndef INCLUDE_COSY_PROPAGATOR_H_
#define INCLUDE_COSY_PROPAGATOR_H_


#include "cosy/Literal.h"
#include "cosy/Clause.h"

namespace cosy {

class SPFSPropagator {
 public:
    SPFSPropagator() : _permutation(nullptr), _propagate(kNoLiteralIndex) {}
    ~SPFSPropagator() {}

    void assign(PermutationSPFS* permutation, LiteralIndex propagate) {
        _permutation = permutation;
        _propagate = propagate;
    }

    void clear() {
        _permutation = nullptr;
        _propagate = kNoLiteralIndex;
    }

    bool canPropagate() const {
        return _permutation != nullptr && _propagate != kNoLiteralIndex;
    }

    PermutationSPFS* permutation() { return _permutation; }

 private:
    PermutationSPFS *_permutation;
    LiteralIndex _propagate;
};

}  // namespace cosy

#endif  // INCLUDE_COSY_PROPAGATOR_H_
