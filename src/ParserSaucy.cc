/* Copyright 2017 Hakan Metin - All rights reserved */

#include <iostream>

#include "cosy/ParserSaucy.h"

using cosy::ParserSaucy;
using cosy::Permutation;

ParserSaucy::ParserSaucy() : Parser() {
}

ParserSaucy::~ParserSaucy() {
}

std::vector< std::unique_ptr<Permutation> >
ParserSaucy::parse(unsigned int num_vars, const std::string symmetryFile) {
    std::vector< std::unique_ptr<Permutation> > generators;
    std::unique_ptr<Permutation> generator;

    StreamBuffer in(symmetryFile);
    int parsed, lit;

    if (! in.valid()) {
        std::clog << "WARNING: Symmetry File " << symmetryFile << " not exists" <<
            std::endl;
        return generators;
    }

    assert(*in == '[');
    ++in;
    assert(*in == '\n');
    ++in;
    if (*in == ']') {  // No generator exists
        return generators;
    }

    while (*in != '\n') {
        assert(*in == '(');
        generator = std::unique_ptr<Permutation>(new Permutation(num_vars));

        while (*in == '(') {
            ++in;
            parsed = in.parseInt();
            lit = graphNodeToLit(parsed, num_vars);
            if (lit != LIT_UNDEF)
                generator->addToCurrentCycle(lit);

            while (*in != ')') {
                assert(*in == ',');
                ++in;
                parsed = in.parseInt();
                lit = graphNodeToLit(parsed, num_vars);
                if (lit != LIT_UNDEF)
                    generator->addToCurrentCycle(lit);
            }
            assert(*in == ')');
            generator->closeCurrentCycle();
            ++in;
        }
        generators.emplace_back(generator.release());
        /* End of Generator line */
        if (*in == ',') {
            ++in;
            assert(*in == '\n');
            ++in;
        }
    }
    ++in;
    assert(*in == ']');

    return generators;
}
