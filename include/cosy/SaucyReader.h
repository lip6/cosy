#ifndef INCLUDE_COSY_SAUCYREADER_H_
#define INCLUDE_COSY_SAUCYREADER_H_

#include <memory>
#include <string>

#include "cosy/Permutation.h"
#include "cosy/PermutationGroup.h"
#include "cosy/StreamBuffer.h"

namespace cosy {

static inline int graphNodeToInt(int node, int num_vars) {
    if (node <= num_vars)
        return node;
    if (node <= 2*num_vars)
        return num_vars - node;
    else
        return 0;  // represent clause
}

class SaucyReader {
 public:
    SaucyReader() {}
    ~SaucyReader() {}

    bool load(unsigned int num_vars,
              const std::string& symmetry_filename,
              PermutationGroup *group);

};


}  // namespace cosy

#endif  // INCLUDE_COSY_SAUCYREADER_H_
