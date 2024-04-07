#!/bin/bash

cd ..

make clean

cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17 --fresh . 

cmake --build . -j 12 

ctest

cd Scripts
