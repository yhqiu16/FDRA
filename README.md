FDRA Framework
=======================

Framework for a hybrid system with a RISC-V core and a Dynamically Reconfigurable Accelerator, including:

1. CGRA-MG: DRA+RISC-V SoC modeling

2. App-Compiler: LLVM-based DFG generator

3. CGRA-Compiler: CGRA mapper and task scheduler.

4. Benchmarks: C programs with target loop kernel annotated


## Build and run flow of FDRA in Chipyard


### Dependencies


#### Chipyard 1.5.0

1. Install dependence tools

```sh
    sudo apt-get install -y build-essential bison flex software-properties-common curl
    sudo apt-get install -y libgmp-dev libmpfr-dev libmpc-dev zlib1g-dev vim default-jdk default-jre
    sudo apt-get install -y texinfo gengetopt
    sudo apt-get install -y libexpat1-dev libusb-dev libncurses5-dev cmake
    sudo apt-get install -y python3.8 patch diffstat texi2html texinfo subversion chrpath wget
    sudo apt-get install -y libgtk-3-dev gettext
    sudo apt-get install -y python3-pip python3.8-dev rsync libguestfs-tools expat ctags
    sudo apt-get install -y device-tree-compiler
```

2. Clone repo and build

```sh
    git clone https://github.com/ucb-bar/chipyard.git
    cd chipyard
    git checkout 1.5.0
    ./scripts/init-submodules-no-riscv-tools.sh
    ./scripts/build-toolchains.sh esp-tools --ignore-qemu
```  
For other details about installing chipyard, please refer to chipyard official documentation:
https://chipyard.readthedocs.io/en/1.5.0/Chipyard-Basics/Initial-Repo-Setup.html

#### LLVM-10.0.0 with Polly included


1. Download llvm source codes from https://github.com/llvm/llvm-project/archive/refs/tags/llvmorg-10.0.0.tar.gz
    
   Extract and change directory name

```sh
    tar xvf llvmorg-10.0.0.tar.gz
    mv llvmorg-10.0.0 llvm-project-10.0.0
```

2. Build llvm

```sh
    mkdir llvm-10.0.0-built
    cd llvm-project-10.0.0
    mkdir build
    cd build
    cmake -DLLVM_ENABLE_PROJECTS='polly;clang' -G "Unix Makefiles" ../llvm
    # multi-thread consumes lots of memory, e.g. -j4 : 30G+
    make -j4
    # DESTDIR set install directory
    make install DESTDIR=/xxx/llvm/llvm-10.0.0-built
```

3. Set llvm env

```sh
    # add following env to .bashrc and then source ~/.bashrc.
    # or directly export the env
    export LLVM_HOME=/xxx/llvm/llvm-10.0.0-built/usr/local/bin
    export PATH=$LLVM_HOME:$PATH
    # or set the LLVM path in the CMakeLists.txt
    set(LLVM_INCLUDE_DIRS "/xxx/llvm-10.0.0-built/usr/local/include")
    set(LLVM_LIBRARY_DIRS "/xxx/llvm-10.0.0-built/usr/local/lib")
```


### CGRA-MG


#### Setup


1. Copy this repo to the chipyard/generators/ directory

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

3. Copy config file to chipyard

```sh
    cd chipyard/generators/fdra
    ./scripts/setup-paths.sh
```

#### Build and run

1. Build verilog

```sh
    source env.sh
    cd generators/fdra
    ./scripts/build-verilog.sh
```

2. Build simulator based on Verilator

```sh
    ./scripts/build-verilator.sh
```

### App-Compiler
#### C Code tagging rules
We use a specific function token(please_map_me()) to help App-Compiler to identifie the target loops. The usage of function token is as follows:
```sh
    __attribute__((noinline))   void convolution2d() {
        for (int i = 1; i < NI -1; i++) {
            for (int j = 1; j < NJ-1; j++) {
            #ifdef CGRA_COMPILER
            please_map_me();
            #endif
            B[i*NJ + j] = c11 * A[(i - 1)*NJ + (j - 1)]  +  c12 * A[(i + 0)*NJ + (j - 1)]  +  c13 * A[(i + 1)*NJ + (j - 1)]
                    + c21 * A[(i - 1)*NJ + (j + 0)]  +  c22 * A[(i + 0)*NJ + (j + 0)]  +  c23 * A[(i + 1)*NJ + (j + 0)]
                    + c31 * A[(i - 1)*NJ + (j + 1)]  +  c32 * A[(i + 0)*NJ + (j + 1)]  +  c33 * A[(i + 1)*NJ + (j + 1)];
            }
        }
    }

```
#### Setup
    Download LLVM-10.0.0 and build
    Set LLVM env

#### Build
Using the script build.sh in this app-compiler directory. The internal path xxx should be changed.

```sh
    ./build.sh
```
#### Run
1. The benchmarks/test4/compile.sh internal path xxx should be changed.

2. Create a folder to store benchmarks in the  benchmarks/test4 directory.

3. Write your benchmark C code.

4. Run the App-Compiler.

```sh
    cd ./<your_create_folder_name>
    bash ../compile.sh <your_benchmark_C_code_file_name> <your_benchmark_name>

    # Take conv2d_3x3  as an example
    cd ./conv2d_3x3
    bash ../compile.sh convolution2d convolution2d
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
The script is as follows
```sh
    # SPDLOG_LEVEL=trace, debug, info, warn, err, critical, off
    ./build/cgra-compiler SPDLOG_LEVEL=off \
        -c true -m true -o true -t 3600000 -i 20000 \
        # operations.json is a file that describes the collection of operators supported by the hardware
        -p "../cgra-mg/src/main/resources/operations.json" \
        # cgra_adg.json is a description file of the hardware architecture
        -a "../cgra-mg/src/main/resources/cgra_adg.json" \
        # it indicates the location of the Benchmark DFG that needs to be mapped
        -d "../benchmarks/test4/fft/affine.json"
```   
The generated result files are in the same directory as the benchmark.The directory structure is described as follows.

    ├── affine.dot                  // benchmark dot form      
    ├── affine.json                // benchmark json form   
    ├── benchmark.c               // benchmark C code
    ├── cgra_call.txt            // contains interface information for calling hardware during simulation 
    ├── cgra_execute.c          // contains bitstream information for simulation
    ├── mapped_adg.dot         // visual architecture dot after binding the DFG node and the ADG node
    ├── mapped_dfg.dot        //  DFG with delay and other information after mapping
    └── mapped_dfgio.dot     // mapping relationship file between DFG IO node and ADG IOB node      





### Build & Run Application on SoC

#### SoC application code generation

We take conv2d_3x3.c which in generators/fdra/software/tests/bareMetalC as an example to demonstrate how to generate SoC application code.    

    1.Replace the convolution2d() function in this file with the your benchmark function. This function is used to generate reference data to verify the results generated by the hardware.

    2.Modify the init_array() function to match your benchmark. This function is used to generate benchmark input data.

    3.Replace the cgra_execute() function in this file with the cgra_execute() function generated after mapping your benchmark in the cgra_execute.c. This function contains bitstream information for hardware.

    4.Modify the result_check() function to match your benchmark. This function is used to verify the result.

    5.Change the cgra_dout_addr and cgra_din_addr arrays, included in the cgra_call.txt, within the main() function to align with your benchmark.


#### Compile SoC application code with RISC-V toolchain

1. Copy the application file to generators/fdra/software/tests/bareMetalC

2. Add the application name in the Makefile 

3. Compile with the generators/fdra/software/tests/build.sh file. 


#### Run SoC application with simulator

```sh
    cd generators/fdra
    ./scripts/run-verilator.sh app-name
```