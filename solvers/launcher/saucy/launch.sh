#!/bin/bash
# Compute symmetry with saucy and launch given solver in parameters
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

P=$(dirname $0)

TIMEOUT=1000

/usr/bin/time -f "%e %M" sh -c "timeout -k $TIMEOUT -s SIGKILL $TIMEOUT \
$P/saucy_breakid $CNF -no-row -no-bin -no-small -no-relaxed \
-with-generator-file -t $TIMEOUT > /dev/null; $P/breakid2saucy $CNF.sym"

$SOLVER_BIN $CNF -sym-file="$CNF.sym" $@

rm "$CNF.sym"
