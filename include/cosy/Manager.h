/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_MANAGER_H_
#define INCLUDE_DSB_MANAGER_H_

#include <iostream>
#include <vector>
#include <unordered_set>
#include <memory>

#include "cosy/Permutation.h"
#include "cosy/Symmetry.h"
#include "cosy/Assigns.h"
#include "cosy/Minimality.h"
#include "cosy/Propagator.h"
#include "cosy/LexOrder.h"
#include "cosy/Orbits.h"

#include "cosy/OrderManager.h"

namespace cosy {

class Manager {
 public:
        explicit Manager(unsigned int num_vars);
        virtual ~Manager();

        void addPermutation(std::unique_ptr<Permutation> permutation);
        void notify(Lit lit, unsigned int level);
        void cancel(Lit lit);

        void order(const std::vector<Lit>& order, LexType lex);

        bool minimal(Var *cause);
        void sbp(std::vector<Lit>* reason);

        void activatePropagations() { _opt_propagator_active = true; }
        bool propagate();
        void propagation(Lit *propagate, std::vector<Lit>* reason);

        void debugPrintPermutations();
        void debugPrintWatchers();
        void debugPrintSymmetries();

        void printStats();
        uint64_t numSymmetricVars() const { return _symmetric_vars.size(); }
        uint64_t numGenerators()    const { return _permutations.size();   }
        uint64_t numConflicts()     const { return _num_conflicts;         }
        uint64_t numPropagations()  const { return _num_propagations;      }

        const LexOrder& lexOrder() const { return _lex_order; }

        const std::vector< std::unique_ptr<Permutation> >&
        permutations() const { return _permutations; }

 private:
        const unsigned int _num_vars;
        Assigns _assigns;

        std::vector< std::unique_ptr<Permutation> > _permutations;
        std::vector< std::unique_ptr<Symmetry> > _symmetries;
        std::vector< std::unordered_set<int> > _watchers;

        LexOrder _lex_order;
        Minimality _minimality;
        Propagator _propagator;

        // Options
        bool _opt_propagator_active;

        // Stats
        std::unordered_set<Var> _symmetric_vars;
        uint64_t _num_conflicts;
        uint64_t _num_propagations;
};

}  // namespace cosy

#endif  // INCLUDE_DSB_MANAGER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
