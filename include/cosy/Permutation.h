/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_PERMUTATION_H_
#define INCLUDE_DSB_PERMUTATION_H_

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>

namespace cosy {

#define NOT_COMPUTED -1

class Permutation {
 public:
        explicit Permutation(unsigned int size) : _size(size), _order(NOT_COMPUTED) {}
        ~Permutation() {}

        void addToCurrentCycle(int x);
        void closeCurrentCycle();

        bool contains(int x) const;
        int order();

        unsigned int size() const { return _size; }
        int numCycles() const { return _cycles_lim.size(); }
        bool isIdentity() const { return _cycles.empty(); }

        /* Example of iterator -- base on or-tools permutation
           ref: https://github.com/google/or-tools.git

        for (int c=0; c<permutation->numCycles(); ++c) {
            int element = permutation->lastElementInCycle(c);
            for (const int image : permutation->cycle(c)) {
                // we have pair element, image

                element = image;
            }
        }
        */
        const std::vector<int>& support() const { return _cycles; }
        struct Iterator;
        Iterator cycle(int i) const;
        int lastElementInCycle(int i) const;
        void debugPrint();

 private:
        const int _size;
        int _order;
        std::vector<int> _cycles;
        std::vector<int> _cycles_lim;
        std::unordered_set<int> _contains;
};

struct Permutation::Iterator {
        typedef int value_type;
        typedef std::vector<int>::const_iterator const_iterator;

        Iterator() {}
        Iterator(const std::vector<int>::const_iterator& b,
                 const std::vector<int>::const_iterator& e) :
            _begin(b), _end(e) {}

        std::vector<int>::const_iterator begin() const { return _begin; }
        std::vector<int>::const_iterator end() const { return _end; }
        const std::vector<int>::const_iterator _begin;
        const std::vector<int>::const_iterator _end;

        int size() const { return _end - _begin; }
};

}  // namespace cosy
#endif  // INCLUDE_DSB_PERMUTATION_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
