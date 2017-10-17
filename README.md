# cosy

cosy is a library that exploits the symmetries of a SAT problem
to cut off isomorphic parts of the search tree. It is a C++ implementation that can be virtually
interfaced with any CDCL SAT solver.

### Files:

```
cosy/
  adapters/               <- Some adapters
  include/                <- Headers files
  lib/                    <- Libraries files
  LICENCE.md              <- Licence file
  Makefile                <- Top Level Makefile
  makefiles/              <- Subsidiary makefiles
  README.md               <- This file
  scripts/                <- Some scripts
  solvers/                <- Some adapted solvers
  solvers/minisat/        <- Adapted Minisat SAT solver
  solvers/glucose-3.0/    <- Adapted Glucose 3.0 SAT solver
  launcher/               <- Store symmetry computer tools
  launcher/bliss/         <- Store bliss symmetry computer tools
  launcher/saucy/         <- Store saucy symmetry computer tools
  solvers/                <- Some adapted solvers
  src/                    <- Source code
  tests/                  <- Some tests
```

### Build

To install cosy locally:
```
make install
```
it creates a ```lib``` directory with the static cosy library and the headers files.


### Running the tests

```
make run-test
```
Runs a set of unit tests that are based on [googletest](https://github.com/google/googletest) library.
dependencies will be downloaded in ```third_party``` directory.

## License

This project is licensed under the GPLv3 - see the LICENSE.md file for details.

## Some solvers with cosy integrated

Integrated solvers are stored in ```solvers``` directory.

### Minisat

Original source code of glucose-3.0 can be found [here](http://minisat.se/)

#### Compile

Execute following commands

```
cd solvers/minisat/core/
export MROOT=..
export LIBRARY_PATH='path_of_cosy_lib'
make
```

### Glucose-3.0

Original source code of glucose-3.0 can be found [here](http://www.labri.fr/perso/lsimon/glucose/)

#### Compile

Execute following commands

```
cd solvers/glucose-3.0/core/
export MROOT=..
export LIBRARY_PATH='path_of_cosy_lib'
make
```

## Launch solver (computation of symmetries included)

With Bliss:

```
./launcher/bliss/launch.sh path_of_solver path_of_cnf_instance
```
Bliss automorphism tool is used, see Bliss on LICENCE.md file.


With Saucy3:

```
./launcher/saucy/launch.sh path_of_solver path_of_cnf_instance
```
The graph generated to found symmetries is based on BreakID implementation.
See BreakID in LICENCE.md files.

Saucy graph automorphism tool is used, see Saucy3 on LICENCE.md file.
