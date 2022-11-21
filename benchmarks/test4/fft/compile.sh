export LLVM_HOME=/home/tallanlong/llvm-10.0.0-built/usr/local/bin
export PATH=$LLVM_HOME:$PATH

clang -D CGRA_COMPILER -target i386-unknown-linux-gnu -c -emit-llvm -O2 -fno-tree-vectorize fft.c -S -o fft.ll

opt -gvn -mem2reg -memdep -memcpyopt -lcssa --indvars -loop-simplify -licm -loop-deletion -simplifycfg -mergereturn fft.ll -S -o fft_gvn.ll

opt -load ../../build/llvm-pass/libCDFGPass.so -fn fft4 -cdfg fft_gvn.ll -S -o fft_cdfg.ll

