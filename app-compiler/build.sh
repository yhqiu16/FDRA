# set env for built LLVM
export LLVM_HOME=/xxx/llvm-10.0.0-built/usr/local/bin
export PATH=$LLVM_HOME:$PATH

# cmake & make
mkdir -p ./build
cd build
cmake ..
make all
