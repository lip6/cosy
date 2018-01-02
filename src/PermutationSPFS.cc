#include "cosy/PermutationSPFS.h"

namespace cosy {

PermutationSPFS::PermutationSPFS(const unsigned int permutation_index,
                                 const Assignment& assignment,
                             const std::unique_ptr<Permutation>& permutation) :
    _permutation_index(permutation_index),
    _assignment(assignment) {

    Literal element;
    for (unsigned int c = 0; c < permutation->numberOfCycles(); c++) {
        element = permutation->lastElementInCycle(c);
        for (const Literal& image : permutation->cycle(c)) {

            _image[element] = image;
            _inverse[image] = element;

            element = image;
        }
    }

}


void PermutationSPFS::updateNotify(Literal literal, const Clause& reason) {

}
void PermutationSPFS::updateCancel(Literal literal) {

}


Literal PermutationSPFS::inverseOf(Literal literal) const {
    DCHECK(_inverse.find(literal) != _inverse.end());
    return _inverse.at(literal);
}

Literal PermutationSPFS::imageOf(Literal literal) const {
    DCHECK(_image.find(literal) != _image.end());
    return _image.at(literal);
}


bool PermutationSPFS::isActive() const {
    return false;
}

bool PermutationSPFS::isInactive() const {
    return false;

}

}  // namespace cosy
