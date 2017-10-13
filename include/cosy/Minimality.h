/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_MINIMALITY_H_
#define INCLUDE_DSB_MINIMALITY_H_

#include <vector>
#include <deque>

#include "cosy/Types.h"

namespace cosy {

class Minimality {
 public:
        Minimality() : _cause(VAR_UNDEF) {}
        ~Minimality() {}

        void add(Lit cause, const std::vector<Lit>& reason) {
            if (_cause != VAR_UNDEF)
                assert(_cause == varOf(cause));

            _conflict_clauses.push_back
                (MinimalityInfo(varOf(cause), reason));
            _cause = varOf(cause);
        }

        void removeIfCause(Lit canceled) {
            if (varOf(canceled) == _cause) {
                _conflict_clauses.clear();
                _cause = VAR_UNDEF;
            }
        }

        bool minimal() const {
            return _conflict_clauses.empty();  // _cause == VAR_UNDEF;
        }

        void sbp(std::vector<Lit>* sbp) {
            assert(_conflict_clauses.size() > 0);

            MinimalityInfo info = _conflict_clauses.front();
            if (sbp != nullptr)
                *sbp = info.sbp;

            _conflict_clauses.pop_front();

            if (_conflict_clauses.empty())
                _cause = VAR_UNDEF;
        }

        Var cause() const { return _cause; }

 private:
        struct MinimalityInfo {
                MinimalityInfo(Var c, const std::vector<Lit>& s):
                    cause(c), sbp(s) {}

                Var cause;
                std::vector<Lit> sbp;
        };
        Var _cause;
        std::deque<MinimalityInfo> _conflict_clauses;
};

}  // namespace cosy
#endif  // INCLUDE_DSB_MINIMALITY_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
