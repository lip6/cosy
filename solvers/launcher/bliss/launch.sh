#!/bin/bash
# Compute symmetry with bliss and launch given solver in parameters
# Note: the solver must take '-sym_fil=...' as option
# Copyright 2017 Hakan Metin - All rights reserved


if [ $# -lt 2 ]; then
    echo "Usage $0: SOLVER_BIN CNF SOLVER_OPTIONS"
    echo "SOLVER_BIN must takes option: -sym-file=..."
    exit
fi

SOLVER_BIN=$1
CNF=$2
shift
shift

echo $SOLVER_BIN $CNF

P=$(dirname $0)

CNF_SYM=$(mktemp $CNF-XXXXXX.sym)
CNF_GRAPH=$(mktemp $CNF-XXXXXX.graph)
TIMEOUT=1000

/usr/bin/time -f "%e %M" sh -c "$P/cnf2graph -dimacs $CNF > $CNF_GRAPH ; \
timeout -k $TIMEOUT -s SIGKILL $TIMEOUT $P/bliss -v=0 $CNF_GRAPH | \
$P/bliss2saucy > $CNF_SYM"

$SOLVER_BIN $CNF -sym-file="$CNF_SYM" $@

rm "$CNF_SYM"
rm "$CNF_GRAPH"
