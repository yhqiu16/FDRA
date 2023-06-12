### Run to generate CDFGs for benchmarks

#### 1. Setup path

change paths of the LLVM library and the generated Pass library in compile.sh


#### 2. Generate DFGs

Using the Makefile in the fdra/benchmarks/test4 directory

The generated CDFG files are in the same dirrectory as the benchmark.

```sh
# generate CDFGs for all benchmarks
make all
# generate CDFG individually
make conv2d_3x3
```
