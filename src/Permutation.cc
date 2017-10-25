/* Copyright 2017 Hakan Metin - All rights reserved*/

#include "cosy/Permutation.h"

using cosy::Permutation;


void Permutation::addToCurrentCycle(int x) {
    _cycles.push_back(x);
    _contains.insert(x);
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

bool Permutation::contains(int x) const {
    return _contains.find(x) != _contains.end();
}

/* gcd and lcm is integrated to C++17 */
int gcd(int a, int b) {
    for (;;) {
        if (a == 0)
            return b;
        b %= a;
        if (b == 0)
            return a;
        a %= b;
    }
}

int lcm(int a, int b) {
    int temp = gcd(a, b);

    return temp ? (a / temp * b) : 0;
}

int Permutation::order() {
    if (_order != NOT_COMPUTED)
        return _order;

    _order = cycle(0).size();
    for (int c = 1; c < numCycles(); ++c) {
        _order = lcm(_order, cycle(c).size());
    }

    return _order;
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
    std::clog << std::endl << "order " << order();
    std::clog << std::endl;

}
