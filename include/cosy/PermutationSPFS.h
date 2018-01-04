#ifndef INCLUDE_COSY_PERMUTATIONSPFS_H_
#define INCLUDE_COSY_PERMUTATIONSPFS_H_

#include <unordered_map>
#include <deque>
#include <memory>
#include <vector>

#include "cosy/Assignment.h"
#include "cosy/Clause.h"
#include "cosy/Literal.h"
#include "cosy/Permutation.h"

namespace cosy {

class PermutationSPFS {
 public:
    PermutationSPFS(const unsigned int permutation_index,
                    const Assignment& assignment,
                    const std::unique_ptr<Permutation>& permutation);
    ~PermutationSPFS();

    void updateNotify(Literal literal);
    void updateCancel(Literal literal);

    LiteralIndex getFirstAsymetricLiteral();


    void generateSymmetricClause(const std::vector<Literal>&reason,
                                 std::vector<Literal> *implication);

 private:
    const unsigned int _permutation_index;
    const Assignment& _assignment;
    std::unordered_map<Literal, Literal> _image;
    std::unordered_map<Literal, Literal> _inverse;
    std::deque<Literal> _notified;
    unsigned int _lookup_index;

    LiteralIndex _reasonOfInactive;
    int _amountForActive;

    Literal inverseOf(Literal literal) const;
    Literal imageOf(Literal literal) const;

    bool isActive() const;
    bool isInactive() const;

};

}  // namespace cosy
#endif  // INCLUDE_COSY_PERMUTATIONSPFS_H_
