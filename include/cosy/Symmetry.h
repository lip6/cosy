/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_SYMMETRY_H_
#define INCLUDE_DSB_SYMMETRY_H_

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>

#include "cosy/Assigns.h"
#include "cosy/Types.h"
#include "cosy/Permutation.h"
#include "cosy/Minimality.h"
#include "cosy/Propagator.h"
#include "cosy/LexOrder.h"

namespace cosy {

struct LookupInfo {
        LookupInfo(Var v, unsigned int i) : var(v), lookup_index(i) {}
        Var var;
        unsigned int lookup_index;
};

class Symmetry {
        friend class Manager;
 public:
        explicit Symmetry(const Assigns& assigns,
                          const unsigned int permutation_index);

        virtual ~Symmetry();

        bool inactive() const;

        void forwardLookupIndex(Lit lit);
        void backwardLookupIndex(Lit lit);

        void verify(Lit lit, Minimality* minimality, const LexOrder& lex);
        void verify(Lit lit, Propagator* propagator, const LexOrder& lex);

        void debugPrint();

        // Public API for tests

        unsigned int lookup_index() const { return _lookup_index; }
        void addLocalOrder(Lit l) { _lookup_order.push_back(l); }
        void addInverse(Lit element, Lit image) { _inverse[image] = element; }

 private:
        const Assigns& _assigns;
        const int _permutation_index;

        std::unordered_map<Lit, Lit> _inverse;

        std::vector<Lit> _lookup_order;
        unsigned int _lookup_index;
        std::deque<LookupInfo> _lookup_trail;

        Lit _blocked_lit;

        void generateSBP(Lit lit, Minimality* minimality);
        void generateSP(Lit undef, Lit cause, Propagator* propagator);

        // Stats

        uint64_t _num_conflicts;
};

}  // namespace cosy
#endif  // INCLUDE_DSB_SYMMETRY_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
