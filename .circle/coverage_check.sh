#!/usr/bin/env bash

export PATH="/usr/local/bin:$PATH"
export PATH="/usr/lib/llvm-3.8/bin:$PATH"

export CMAKE_C_COMPILER=$(which gcc)
export CMAKE_CXX_COMPILER=$(which g++)
export CPP=$(which g++)
export CC=$(which gcc)

echo "Clone submodules"
git submodule update --init --recursive
echo "  clone submodules done"

cd build
make clean
cmake -DCMAKE_C_COMPILER=$(which gcc) -DCMAKE_BUILD_TYPE=Debug ..
make -j20
make -j20 test_kore_query_coverage
