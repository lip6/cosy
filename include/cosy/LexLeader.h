#ifndef INCLUDE_COSY_LEXLEADER_H_
#define INCLUDE_COSY_LEXLEADER_H_

#include <memory>

#include "cosy/Assignment.h"
#include "cosy/Order.h"

namespace cosy {

class LexLeader {
 public:
    LexLeader() : _esbp(nullptr), _reason(kNoBooleanVariable) {}
    ~LexLeader() {}

    void add(const std::vector<Literal>& clause, const BooleanVariable& reason) {
        _esbp = std::unique_ptr<Clause>(new Clause(clause));
        _reason = reason;
    }

    void removeIfReason(const Literal& literal) {
        if (literal.variable() == _reason) {
            _esbp = nullptr;
            _reason = kNoBooleanVariable;
        }
    }

    bool isNotLexLeader() const { return _esbp != nullptr; }
    bool isNotLexLeader(const Literal& reason) const {
        return isNotLexLeader() && reason.variable() == _reason;
    }
    const std::unique_ptr<Clause>& esbp() { return _esbp; }

 private:
    std::unique_ptr<Clause> _esbp;
    BooleanVariable _reason;
};

}  // namespace cosy

#endif  // INCLUDE_COSY_LEXLEADER_H_
