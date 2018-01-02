/* Copyright 2017 Hakan Metin - All rights reserved*/

#include "cosy/Permutation.h"

namespace cosy {

void Permutation::addToCurrentCycle(Literal x) {
    _cycles.push_back(x);
}

void Permutation::closeCurrentCycle() {
    int sz = _cycles.size();
    int last;

    if (_cycles_lim.empty()) {
        if (sz > 1)
            _cycles_lim.push_back(sz);
    } else {
        last = _cycles_lim.back();
        if (last != sz) {
            assert(sz - last >= 2);
            _cycles_lim.push_back(sz);
        }
    }
}

Permutation::Iterator Permutation::cycle(int i) const {
    DCHECK_GE(i, 0);
    DCHECK_LT(i, numberOfCycles());

    return Iterator(_cycles.begin() + (i == 0 ? 0 : _cycles_lim[i - 1]),
        _cycles.begin() + _cycles_lim[i]);
}

Literal Permutation::lastElementInCycle(int i) const {
    DCHECK_GE(i, 0);
    DCHECK_LT(i, numberOfCycles());

    return _cycles[_cycles_lim[i] - 1];
}

void Permutation::debugPrint() {
    for (unsigned int c = 0; c < numberOfCycles(); ++c) {
        std::cout << "(";
        for (const Literal& element : cycle(c)) {
            if (element == lastElementInCycle(c))
                std::cout << element.signedValue();
            else
                std::cout << element.signedValue() << " ";
        }
        std::cout << ")";
    }
    std::cout << std::endl;
}

}  // namespace cosy
