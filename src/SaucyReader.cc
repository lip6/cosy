
#include "cosy/SaucyReader.h"

namespace cosy {

bool SaucyReader::load(unsigned int num_vars,
                       const std::string& symmetry_filename,
                       PermutationGroup *group) {

    std::unique_ptr<Permutation> generator;
    StreamBuffer in(symmetry_filename);
    int parsed, cnf_int;
    Literal lit;

    if (!in.isValid())
        LOG(FATAL) << in.errorMessage() << std::endl;

    assert(*in == '[');
    ++in;
    assert(*in == '\n');
    ++in;

    if (*in == ']') {  // No generator exists
        return true;
    }

    while (*in != '\n') {
        assert(*in == '(');
        generator = std::unique_ptr<Permutation>(new Permutation(num_vars));

        while (*in == '(') {
            ++in;
            parsed = in.readInt();
            cnf_int = graphNodeToInt(parsed, num_vars);
            if (cnf_int != 0) {
                lit = Literal(cnf_int);
                generator->addToCurrentCycle(lit);
            }

            while (*in != ')') {
                assert(*in == ',');
                ++in;
                parsed = in.readInt();
                cnf_int = graphNodeToInt(parsed, num_vars);
                if (cnf_int != 0) {
                    lit = Literal(cnf_int);
                    generator->addToCurrentCycle(lit);
                }
            }

            assert(*in == ')');
            generator->closeCurrentCycle();
            ++in;
        }
        group->addPermutation(std::move(generator));
        /* End of Generator line */
        if (*in == ',') {
            ++in;
            assert(*in == '\n');
            ++in;
        }
    }
    ++in;
    assert(*in == ']');

    return true;
}


}  // namespace cosy
