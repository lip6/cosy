/* Copyright 2017 Hakan Metin - All rights reserved */

#include <iostream>

#include "cosy/CNFModel.h"
#include "cosy/Printer.h"

using cosy::CNFModel;


void CNFModel::debugPrint() {
    std::cout << "p cnf " << numVars() << " " << numClauses() << std::endl;
    for (const Clause& clause : _clauses) {
        for (const Lit& lit : clause) {
            std::cout << lit << " ";
        }
        std::cout << "0" << std::endl;
    }
}

void CNFModel::printStats() {
    Printer::printStat("number of variables", numVars());
    Printer::printStat("number of clauses", numClauses());
    Printer::printStat(" |- unary clauses", numUnary(), numClauses());
    Printer::printStat(" |- binary clauses", numBinary(), numClauses());
    Printer::printStat(" |- ternary clauses", numTernary(), numClauses());
    Printer::printStat(" |- other clauses",
                       numClauses() - numUnary() - numBinary() - numTernary(),
                        numClauses());
    Printer::printStat("ratio variables / clauses", numVars(), numClauses());
}
