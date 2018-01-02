#ifndef INCLUDE_COSY_LEXLEADERFORCING_H_
#define INCLUDE_COSY_LEXLEADERFORCING_H_

#include <memory>

#include "cosy/Assignment.h"
#include "cosy/PermutationStatus.h"
#include "cosy/Order.h"

namespace cosy {

class LexLeaderForcing {
 public:
    LexLeaderForcing() : _esbps(), _reason(kNoBooleanVariable) {}
    ~LexLeaderForcing() {}

    void registerESBP(std::unique_ptr<Clause>& esbp, Literal reason) {
        if (_reason != kNoBooleanVariable)
            DCHECK(reason.variable() == _reason);

        _esbps.emplace_back(esbp.release());
        _reason = reason.variable();
    }

    void removeIfReason(Literal literal) {
        if (literal.variable() == _reason) {
            _esbps.clear();
            _reason = kNoBooleanVariable;
        }
    }

    bool isESBPExist() const { return _esbps.size() > 0; }
    bool isESBPExist(Literal reason) const {
        return isESBPExist() && reason.variable() == _reason;
    }

    std::unique_ptr<Clause> ESBP() {
        std::unique_ptr<Clause> esbp = std::move(_esbps.front());
        _esbps.erase(_esbps.begin());
        if (! isESBPExist())
            _reason = kNoBooleanVariable;
        return esbp;
    }
    BooleanVariable reason() const { return _reason; }

 private:
    std::vector<std::unique_ptr<Clause>> _esbps;
    BooleanVariable _reason;
};

}  // namespace cosy

#endif  // INCLUDE_COSY_LEXLEADERFORCING_H_
