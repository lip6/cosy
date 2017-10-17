# cosy

cosy is a Controller of symmetry.
It provide to an existing SAT solver effective symmetry breaking
predicates to cut off isomorphic search space.

### Files

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

### Building

To compile cosy:

```
make
```

### Install

```
make install
```

It create a ```lib``` directory with static library and include files.

### Running the tests

```
make run-test
```

## cosy and Integration in Minisat

### Model of cosy

```
Variable : Var    unsigned int            Start from 1. 0 means variable is undef
Literal  : Lit    int                     Start from 1 ( -1) 0 means literal is undef.
Clause   : Clause std::vector<Lit>        A C++ vector of Literal
```
### Adapter

Because cosy, use its own model of variable, literal, clause.
Solver must be conform to ``` include/cosy/LiteralAdapter.h``` interface.
This implements 2 methods to adapt a literal from cosy and to cosy.

Example: Code of Minisat Adapter
```
namespace Minisat {

class MinisatLiteralAdapter : public cosy::LiteralAdapter<Minisat::Lit>
{
 public:
        MinisatLiteralAdapter() {}
        ~MinisatLiteralAdapter() {}

        virtual Minisat::Lit convert(cosy::Lit l) {
            return mkLit(cosy::varOf(l) - 1, cosy::sign(l));
        }

        virtual cosy::Lit convert(Minisat::Lit lit) {
            return (sign(lit) ? -(var(lit) + 1) : var(lit) + 1);
        }
};
}
```

* Adapter of [Minisat](http://minisat.se/) is available at ```adapters/MinisatLiteralAdapter.h```
* Adapter of [Glucose 3.0](http://www.labri.fr/perso/lsimon/glucose/) is available at ```adapters/GlucoseLiteralAdapter.h```

### cosy options

* Activate or Deactivate Forcing Lex Leader.

```
 symmetry->activateLexLeaderForcing();
```

* Order Options
```
symmetry->order(order_type, value_type);
```
With order_type:

* ```cosy::OrderType::INCREASING``` 1 2 3 ... n
* ```cosy::OrderType::CLASS_OCCURENCE``` pack variables acording to their orbit, sorted their occurence
* ```cosy::OrderType::OCCURENCE```  sort according to number of occurence of each variables
We can also define a custom order.

And value_type:

* ```cosy::T_LESS_F```
* ```cosy::F_LESS_T```


Note that ```cosy::OrderType::OCCURENCE``` with  ```cosy::T_LESS_F``` works relatively good.

### Initialisation of Symmetry Controller

File ```core/Main.cc```
First Create the adapter:

```
        std::unique_ptr<cosy::LiteralAdapter<Minisat::Lit>> adapter
            (new MinisatLiteralAdapter());
```

Create the symmetry Controller with the adapter and number of variables.
```
        S.symmetry = std::unique_ptr<cosy::SymmetryController<Minisat::Lit>>
            (new cosy::SymmetryController<Minisat::Lit>(S.nVars(), adapter));

```

Initialize the symmetry controller with cnf instance and symmetry generator file.
```
        std::string symmetry_file = std::string(sym_file);
        std::string cnf_file = std::string(argv[1]);

        if (!S.symmetry->initialize(cnf_file, symmetry_file))
            S.symmetry = nullptr;

```

### Configuration of Symmetry Controller

File ```core/Solver.cc```

```
lbool Solver::solve_()
    ...
    // Set symmetry order
    if (symmetry != nullptr) {
        // symmetry->activateLexLeaderForcing();
        symmetry->order(cosy::OrderType::OCCURENCE, cosy::T_LESS_F);
        symmetry->printInfo();
    }
```
### Integration of API

File ```core/Solver.cc```


Update Symmetry status on affectation:
```
void Solver::uncheckedEnqueue(Lit p, CRef from) {
    ...
    if (symmetry != nullptr)
        symmetry->updateNotify(p, decisionLevel());
}
```

Update Symmetry status on cancel:
```
void Solver::cancelUntil(int level) {
    ...
     assigns [x] = l_Undef;
	 if (symmetry != nullptr)
        symmetry->updateCancel(trail[c]);
}
```

Generate ESBP when in conflict before starting to propagate.
Generate Forcing ESBP after classic unit propagation.
```
CRef Solver::propagate() {

        ...
        num_props++;
        /* Learning of Esbp */
        if (symmetry != nullptr) {
            if (symmetry->isNotLexLeader(p)) {
                /* Generate SBP */
                std::vector<Lit> vsbp = symmetry->generateEsbp();

                // Dirty make a copy of vector
                vec<Lit> sbp;
                for (Lit l : vsbp)
                    sbp.push(l);

                if (sbp.size() == 1) {
                    cancelUntil(0);
                    uncheckedEnqueue(sbp[0]);
                } else {
                    CRef cr = ca.alloc(sbp, true);
                    learnts.push(cr);
                    attachClause(cr);
                }
            }
        }
        ...


        ws.shrink(i - j);
        /* Forcing Lex Leader */
        if (confl == CRef_Undef && qhead == trail.size() && symmetry != nullptr) {
	    while (symmetry->canForceLexLeader()) {
		Lit propagate;
		vec<Lit> reason;
                std::vector<Lit> r;

                r = symmetry->generateForceLexLeaderEsbp(&propagate);
                // Copy can be avoided
                for (Lit l : r)
                    reason.push(l);

		if (value(propagate) != l_Undef)
		    continue;
		CRef cr = ca.alloc(reason, true);
		learnts.push(cr);
		attachClause(cr);
 		uncheckedEnqueue(propagate, cr);
	    }
	}

}

```

## Some solvers with cosy integrated

### Minisat

#### Compile

Execute following commands

```
cd solvers/minisat/core/
export MROOT=..
export LIBRARY_PATH='path_of_cosy_lib'
make
```


### Glucose-3.0

#### Compile

Execute following commands

```
cd solvers/glucose-3.0/core/
export MROOT=..
export LIBRARY_PATH='path_of_cosy_lib'
make
```

### Launch solver (computation of symmetries included)

With Bliss:

```
./launcher/bliss/launch.sh path_of_solver path_of_cnf_instance

```

With Saucy3:

```
./launcher/saucy/launch.sh path_of_solver path_of_cnf_instance

```

## License

This project is licensed under the GPLv3 - see the [LICENSE.md](LICENSE.md)
file for details
