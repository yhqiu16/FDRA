
set(PASS_NAME CDFGPass)

file(GLOB_RECURSE SRCS ${CMAKE_SOURCE_DIR}/llvm-pass/src/*.cpp)
add_library(${PASS_NAME} MODULE ${SRCS})
target_include_directories(${PASS_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/llvm-pass/include)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(${PASS_NAME} PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that.
set_target_properties(${PASS_NAME} PROPERTIES
    COMPILE_FLAGS "-fno-rtti -ggdb"
)