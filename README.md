# cosy

cosy is a Controller of symmetry.
It provide to an existing SAT solver effective symmetry breaking
predicates to cut off isomorphic search space.

### Files
    
cosy/  
&nbsp;&nbsp; adapters/               <- Some adapters  
&nbsp;&nbsp; include/                <- Headers files  
&nbsp;&nbsp; lib/                    <- Libraries files  
&nbsp;&nbsp; LICENCE.md              <- Licence file  
&nbsp;&nbsp;Makefile                <- Top Level Makefile  
&nbsp;&nbsp;makefiles/              <- Subsidiary makefiles  
&nbsp;&nbsp;README.md               <- This file  
&nbsp;&nbsp;scripts/                <- Some scripts  
&nbsp;&nbsp;solvers/                <- Some adapted solvers  
&nbsp;&nbsp;solvers/minisat/        <- Adapted Minisat SAT solver  
&nbsp;&nbsp;solvers/glucose-3.0/    <- Adapted Glucose 3.0 SAT solver  
&nbsp;&nbsp;solvers/                <- Some adapted solvers  
&nbsp;&nbsp;src/                    <- Source code  
&nbsp;&nbsp;tests/                  <- Some tests  


### Building

To compile cosy:

```
make
```

### Install

```
make install
```

It create a lib directory with static library and include files.

### Running the tests

```

make run-test
```

## Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D

## License

This project is licensed under the GPLv3 - see the [LICENSE.md](LICENSE.md)
file for details
