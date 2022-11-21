App-Compiler
=======================

Frontend application compiler that generates CDFG from C code based on LLVM


## Getting Started

### Dependencies

1. LLVM-10.0.0 with Polly included

2. CMake

3. C++-14



### LLVM with Polly

1. download llvm source codes from https://github.com/llvm/llvm-project/archive/refs/tags/llvmorg-10.0.0.tar.gz
    
   extract and change directory name

```sh
    tar xvf llvmorg-10.0.0.tar.gz
    mv llvmorg-10.0.0 llvm-project-10.0.0
```

2. build llvm

```sh
    mkdir llvm-10.0.0-built
    cd llvm-project-10.0.0
    mkdir build
    cd build
    cmake -DLLVM_ENABLE_PROJECTS='polly;clang' -G "Unix Makefiles" ../llvm
    # multi-thread consumes lots of memory, e.g. -j4 : 30G+
    make -j4
    # DESTDIR set install directory
    make install DESTDIR=/home/yhqiu/llvm/llvm-10.0.0-built
```

3. set llvm env

```sh
    # add following env to .bashrc and then source ~/.bashrc.
    # or directly export the env
    export LLVM_HOME=/home/yhqiu/llvm/llvm-10.0.0-built/usr/local/bin
    export PATH=$LLVM_HOME:$PATH
    # or set the LLVM path in the CMakeLists.txt
    set(LLVM_INCLUDE_DIRS "/xxx/llvm-10.0.0-built/usr/local/include")
    set(LLVM_LIBRARY_DIRS "/xxx/llvm-10.0.0-built/usr/local/lib")
```

### Build

Using the script build.sh in this app-compiler directory

```sh
./build.sh
```

### Run to generate CDFGs for benchmarks

Using the Makefile in the app-compiler/applications directory

The generated CDFG files are in the same dirrectory as the benchmark.

```sh
# generate CDFGs for all benchmarks
make all
# generate CDFG individually
make gemm
```





