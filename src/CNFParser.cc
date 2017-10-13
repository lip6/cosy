/* Copyright 2017 Hakan Metin - All rights reserved */

#include "cosy/CNFParser.h"

using cosy::CNFParser;

cosy::CNFModel CNFParser::parse(const std::string cnf_file) {
    CNFModel model;

    StreamBuffer in(cnf_file);
    unsigned int expected_num_vars = 0, expected_num_clauses = 0;
    Lit lit;

    while (!in.eof()) {
        in.skipWhiteSpace();

        if (*in == 'c') {
            in.skipLine();
        } else if (*in == 'p') {
            ++in; assert(*in == ' ');
            ++in; assert(*in == 'c');
            ++in; assert(*in == 'n');
            ++in; assert(*in == 'f');
            ++in; assert(*in == ' ');
            expected_num_vars = in.parseInt();
            expected_num_clauses = in.parseInt();
        } else {
            do {
                lit = in.parseInt();
                if (lit == LIT_UNDEF)
                    model.closeCurrentClause();
                else
                    model.addToCurrentClause(lit);
            } while (lit != LIT_UNDEF);
        }

        in.skipWhiteSpace();
    }

    if (model.numVars() != expected_num_vars)
        std::clog << "WARNING: CNF file, expected " << expected_num_vars
                  << " vars found " << model.numVars() << std::endl;
    if (model.numClauses() != expected_num_clauses)
        std::clog << "WARNING: CNF file, expected " << expected_num_clauses
                  << " clauses found " << model.numClauses() << std::endl;

    return model;
}
