#!/bin/bash

mkdir -p build 

cd build 

make clean

cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ --fresh .. 

cmake --build . -j 12 

ctest

cd ..
