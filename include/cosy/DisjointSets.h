/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_DISJOINTSETS_H_
#define INCLUDE_DSB_DISJOINTSETS_H_

#include <vector>
#include <memory>
#include <iostream>
#include <utility>

namespace cosy {

class DisjointSets {
 public:
        DisjointSets() : _num_elements(0), _num_sets(0) {}
        virtual ~DisjointSets() {}

        void Add(unsigned int element);
        void Union(int set1, int set2);
        int  Find(int element);

        unsigned int num_elements() const { return _num_elements; }
        unsigned int num_sets() const { return _num_sets; }

        void debugPrint();

 private:
        struct Node {
                explicit Node(unsigned int n) :
                    parent(nullptr),
                    index(n),
                    rank(0) {}
                Node *parent;
                unsigned int index;
                int rank;
        };

        std::vector< std::unique_ptr<Node> > _nodes;
        unsigned int _num_elements;
        unsigned int _num_sets;
};

inline void DisjointSets::Add(unsigned int element) {
    if (element >= _nodes.size())
        _nodes.resize(element + 1);

    if (_nodes[element] == nullptr) {
        _nodes[element] = std::move(std::unique_ptr<Node>(new Node(element)));
        _num_elements++;
        _num_sets++;
    }
}

inline void DisjointSets::Union(int x, int y) {
    assert(_nodes[x] != nullptr);
    assert(_nodes[y] != nullptr);

    Node *x_root = _nodes[Find(x)].get();
    Node *y_root = _nodes[Find(y)].get();
    int x_rank, y_rank;

    if (x_root == y_root)
        return;

    x_rank = x_root->rank;
    y_rank = y_root->rank;

    if (x_rank < y_rank) {
        x_root->parent = y_root;
    } else if (x_rank > y_rank) {
        y_root->parent = x_root;
    } else { /* x_rank == y_rank */
        y_root->parent = x_root;
        x_root->rank += 1;
    }

    _num_sets--;
}

inline int DisjointSets::Find(int element) {
    Node *root = _nodes[element].get();
    Node *set = nullptr, *parent = nullptr;

    while (root->parent != nullptr) {
        root = root->parent;
    }

    /* Path compression */
    set = _nodes[element].get();
    while (set != root) {
        parent = set->parent;
        set->parent = root;
        set = parent;
    }

    return root->index;
}


inline void DisjointSets::debugPrint() {
    for (unsigned int i = 0; i < _nodes.size(); i++)
        if (_nodes[i] != nullptr)
            std::cout << i << " : " << Find(i) << std::endl;
}

}  // namespace cosy

#endif  // INCLUDE_DSB_DISJOINTSETS_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
