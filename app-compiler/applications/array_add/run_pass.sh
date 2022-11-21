rm -f *_looptrace.log
rm -f *_loopinfo.log
rm -f *_munittrace.log
rm -f memtraces/*
clang -D CGRA_COMPILER -target i386-unknown-linux-gnu -c -emit-llvm -O3 -fno-tree-vectorize -fno-inline -fno-unroll-loops array_add.c -S -o array_add.ll
opt -gvn -mem2reg -memdep -memcpyopt -lcssa -loop-simplify -licm -loop-deletion -indvars -simplifycfg -mergereturn -indvars array_add.ll -S -o array_add_opt.ll

# opt -load ../../build/llvm-pass/libCDFGPass.so -fn array_add -cdfg array_add_opt.ll
opt -load ../../build/llvm-pass/libCDFGPass.so -fn $1 -cdfg array_add_opt.ll -S -o array_add_opt_instrument.ll

# clang -target i386-unknown-linux-gnu -c -emit-llvm -S ../../skeleton/instrumentation/instrumentation.cpp -o instrumentation.ll

# llvm-link array_add_opt_instrument.ll instrumentation.ll -o final.ll

# llc -filetype=obj final.ll -o final.o
# clang++ -m32 final.o -o final
