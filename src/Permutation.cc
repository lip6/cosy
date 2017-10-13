/* Copyright 2017 Hakan Metin - All rights reserved*/

#include "cosy/Permutation.h"

using cosy::Permutation;


void Permutation::addToCurrentCycle(int x) {
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
    assert(i >= 0);
    assert(i < numCycles());
    return Iterator(_cycles.begin() + (i == 0 ? 0 : _cycles_lim[i - 1]),
        _cycles.begin() + _cycles_lim[i]);
}

int Permutation::lastElementInCycle(int i) const {
    assert(i >= 0);
    assert(i < numCycles());
    return _cycles[_cycles_lim[i] - 1];
}

void Permutation::debugPrint() {
    for (int c = 0; c < numCycles(); ++c) {
        std::clog << "(";
        for (const int element : cycle(c)) {
            if (element == lastElementInCycle(c))
                std::clog << element;
            else
                std::clog << element << " ";
        }
        std::clog << ")";
    }
    std::clog << std::endl;
}
