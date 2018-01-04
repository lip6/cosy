#include "cosy/PermutationSPFS.h"

namespace cosy {

PermutationSPFS::PermutationSPFS(const unsigned int permutation_index,
                                 const Assignment& assignment,
                             const std::unique_ptr<Permutation>& permutation) :
    _permutation_index(permutation_index),
    _assignment(assignment),
    _lookup_index(0),
    _reasonOfInactive(kNoLiteralIndex),
    _amountForActive(0) {

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

PermutationSPFS::~PermutationSPFS() {
}

void PermutationSPFS::updateNotify(Literal literal) {
    Literal image = imageOf(literal);
    Literal inverse = inverseOf(literal);

    DCHECK(literal != image);
    DCHECK(_assignment.literalIsTrue(literal));

    _notified.push_back(literal);

    if (isInactive())
        return;

    if (_assignment.isDecision(inverse)) {
        if (_assignment.literalIsTrue(inverse)) {
            --_amountForActive;
        } else {
            DCHECK(_assignment.literalIsFalse(inverse));
            _reasonOfInactive = literal.index();
        }
    }

    if (_assignment.isDecision(literal)) {
        if (!_assignment.literalIsAssigned(image)) {
            ++_amountForActive;
        } else if (_assignment.literalIsFalse(image)) {
            _reasonOfInactive = literal.index();
        }  // else _assignment.literalIsTrue(image)
    }

}

void PermutationSPFS::updateCancel(Literal literal) {
    Literal image = imageOf(literal);
    Literal inverse = inverseOf(literal);

    DCHECK(literal != image);
    DCHECK(_assignment.literalIsAssigned(literal));
    DCHECK(_notified.size() > 0 && _notified.back() == literal);

    _notified.pop_back();
    _lookup_index = 0;

    if (isInactive()) {
        if (_reasonOfInactive == literal.index())
            _reasonOfInactive = kNoLiteralIndex;
        else
            return;
    }

    if (_assignment.isDecision(literal) && !_assignment.literalIsAssigned(image))
        --_amountForActive;

    if (_assignment.isDecision(inverse) && _assignment.literalIsTrue(inverse))
        ++_amountForActive;
}

void PermutationSPFS::generateSymmetricClause(const std::vector<Literal>&reason,
                                            std::vector<Literal> *implication) {

    implication->clear();
    for (const Literal& lit : reason)
        implication->push_back(imageOf(lit));

}


LiteralIndex PermutationSPFS::getFirstAsymetricLiteral() {
    if (! isActive())
        return kNoLiteralIndex;

    for (;  _lookup_index < _notified.size(); ++_lookup_index) {
        const Literal literal = _notified[_lookup_index];
        const Literal image = imageOf(literal);

        if (!(_assignment.isDecision(literal) ||
              _assignment.literalIsTrue(image)))
            break;
    }

    if (_lookup_index == _notified.size())
        return kNoLiteralIndex;


    DCHECK(! _assignment.literalIsTrue(imageOf(_notified[_lookup_index])));
    return _notified[_lookup_index].index();
}

Literal PermutationSPFS::inverseOf(Literal literal) const {
    if (_inverse.find(literal) == _inverse.end())
        return literal;
    return _inverse.at(literal);
}

Literal PermutationSPFS::imageOf(Literal literal) const {
    if (_image.find(literal) == _image.end())
        return literal;
    return _image.at(literal);
}

bool PermutationSPFS::isActive() const {
    return _amountForActive == 0 &&  _reasonOfInactive == kNoLiteralIndex;
}

bool PermutationSPFS::isInactive() const {
    return _reasonOfInactive != kNoLiteralIndex;
}

}  // namespace cosy
