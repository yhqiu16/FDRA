FDRA Framework
=======================

Framework for a hybrid system with RISC-V and CGRA, including:

1. CGRA-MG: SoC modeling

2. App-Compiler: LLVM-based DFG generator

3. CGRA-Compiler: CGRA mapper.

4. Bechmarks: DFGs in Json format derived from dot files.


## Getting Started


### Dependencies

##### JDK 8 or newer (for CGRA-MG)

##### SBT (for CGRA-MG)

##### CMake  (for CGRA-Compiler)

##### C++-11 (for CGRA-Compiler)




### CGRA-MG

#### Build and run

Using the script run.sh
```sh
cd cgra-mg
./run.sh
```

Or using sbt command:
```sh
cd cgra-mg
sbt "runMain mg.CGRAMG -td ./test_run_dir"
```

### App-Compiler


### CGRA-Compiler

#### Build

Using the script build.sh
```sh
cd cgra-compiler
./build.sh
```

#### Run

Using the script run.sh
```sh
./run.sh
```

Change the benchmark file path as you need.
The generated result files are in the same directory as the benchmark.




