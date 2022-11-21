# build
mkdir -p build && cd build
cmake ..
# cmake -DCMAKE_BUILD_TYPE=Release ..
make

# clean
# make -C build clean