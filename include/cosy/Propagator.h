/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_PROPAGATOR_H_
#define INCLUDE_DSB_PROPAGATOR_H_

#include <vector>
#include <deque>
#include <memory>

#include "cosy/Types.h"

namespace cosy {


class Propagator {
 public:
        Propagator() {}
        ~Propagator() {}

        void add(Lit propagate, Lit cause, const std::vector<Lit>& reason) {
            _unit_clauses.push_back
                (std::unique_ptr<PropagatorInfo>
                 (new PropagatorInfo(propagate, cause, reason)));
        }

        void removeIfCause(Lit canceled) {
            while (_unit_clauses.size() > 0 &&
                   varOf(_unit_clauses.back()->cause) == varOf(canceled))
                _unit_clauses.pop_back();
        }

        bool propagate() const {
            return _unit_clauses.size() > 0;
        }

        void propagation(Lit* propagate, std::vector<Lit>* reason) {
            assert(_unit_clauses.size() > 0);

            const std::unique_ptr<PropagatorInfo>& info = _unit_clauses.front();
            if (propagate != nullptr)
                *propagate = info->propagate;
            if (reason != nullptr)
                *reason = info->reason;
            _unit_clauses.pop_front();
        }

        void debugPrint() {
            for (std::unique_ptr<PropagatorInfo>& info : _unit_clauses) {
                std::cout << info->propagate << " "
                          << info->cause << " | ";
            }
            std::cout << std::endl;
        }

 private:
        struct PropagatorInfo {
                PropagatorInfo(Lit p, Lit c, const std::vector<Lit>& r) :
                    propagate(p), cause(c), reason(r) {}

                Lit propagate;
                Lit cause;
                std::vector<Lit> reason;
        };

        std::deque< std::unique_ptr<PropagatorInfo> > _unit_clauses;
};
}  // namespace cosy
#endif  // INCLUDE_DSB_PROPAGATOR_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */

