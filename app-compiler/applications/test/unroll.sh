export LLVM_HOME=/home/tallanlong/llvm-10.0.0-built/usr/local/bin
export PATH=$LLVM_HOME:$PATH

clang -D CGRA_COMPILER -target i386-unknown-linux-gnu -c -emit-llvm -O2 -fno-tree-vectorize -fno-unroll-loops test.c -S -o test.ll

opt -gvn -mem2reg -memdep -memcpyopt -indvars -lcssa -loop-simplify -licm -loop-deletion -indvars -simplifycfg -mergereturn -indvars test.ll -o test_gvn.ll

opt -loop-unroll -unroll-count=3 test_gvn.ll -o test_unroll.ll

opt -load ../../build/llvm-pass/libCDFGPass.so -fn test -cdfg test_unroll.ll -S -o test_cdfg.ll
