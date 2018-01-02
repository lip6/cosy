
#include <gtest/gtest.h>
#include <memory>


#include "cosy/SymmetryController.h"
#include "cosy/IdentityLiteralAdapter.h"


namespace cosy {

TEST(SymmetryControllerTest, Constructor) {
    std::string cnf_filename = "tests/resources/exist.cnf";
    std::string sym_filename = "tests/resources/exist.cnf.saucy";

    std::unique_ptr<LiteralAdapter<Literal>> adapter
        (new IdentityLiteralAdapter());

    SymmetryController<Literal> controller(cnf_filename,
                                           sym_filename,
                                           adapter);

    controller.order(OCCURENCE, TRUE_LESS_FALSE);

}

}  // namespace cosy
