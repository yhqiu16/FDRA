# ./compile.sh c-file-name function-name

# -fno-inline -fno-unroll-loops
export LLVM_HOME=/home/tallanlong/llvm-10.0.0-built/usr/local/bin
export PATH=$LLVM_HOME:$PATH

clang -D CGRA_COMPILER -target i386-unknown-linux-gnu -c -emit-llvm -O2 -fno-tree-vectorize -fno-unroll-loops $1.c -S -o $1.ll

opt -gvn -mem2reg -memdep -memcpyopt -lcssa -loop-simplify -licm -loop-deletion -indvars -simplifycfg -mergereturn -indvars $1.ll -o $1_gvn.ll

opt -load ../../build/llvm-pass/libCDFGPass.so -fn $2 -cdfg $1_gvn.ll -S -o $1_cdfg.ll

