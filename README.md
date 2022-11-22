FDRA Framework
=======================

Framework for a hybrid system with RISC-V and CGRA, including:

1. CGRA-MG: SoC modeling

2. App-Compiler: LLVM-based DFG generator

3. CGRA-Compiler: CGRA mapper.

4. Bechmarks: DFGs in Json format derived from dot files.


## Build and run flow of FDRA in Chipyard


### Dependencies

##### Chipyard

##### LLVM-10.0.0 with Polly included



### CGRA-MG

#### Setup

1. copy this repo to the chipyard/generators/ directory

2. Add the following configuration in the chipyard/build.sbt file

```
lazy val chipyard = (project in file("generators/chipyard"))
  .dependsOn(
    ...
    fdra
)

lazy val fdra = (project in file("generators/fdra/cgra-mg"))
  .dependsOn(testchipip, rocketchip)
  .settings(libraryDependencies ++= rocketLibDeps.value)
  .settings(chiselTestSettings)
  .settings(commonSettings)

```

3. copy config file to chipyard

```sh
cd chipyard/generators/fdra
./scripts/setup-paths.sh
```

#### Build and run

1. build verilog

```sh
source env.sh
cd generators/fdra
./scripts/build-verilog.sh
```

2. build simulator based on Verilator

```sh
./scripts/build-verilator.sh
```

### App-Compiler

#### Setup

Download LLVM-10.0.0 and build

Set LLVM env

#### Build and Run

Using the script build.sh in this app-compiler directory. The internal path should be changed.

```sh
./build.sh
```


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



### Build & Run Application on SoC

#### SoC application code generation

Automatically generate with CGRA Compiler, or

Manually replace the target loop kernel with CGRA call function and binding codes generated by CGRA Mapper


#### Compile SoC application code with RISC-V toolchain

1. copy the application file to generators/fdra/software/tests

2. add the application in the Makefile

3. compile with the build.sh file. 


#### Run SoC application with simulator

```sh
cd generators/fdra
./scripts/run-verilator.sh app-name
```