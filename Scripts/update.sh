#!/bin/bash

systemctl stop UniversalDeviceFrontend

systemctl stop UniversalDevice

git fetch --all

git pull

mkdir -p build 

cd build 

make clean

cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17 --fresh ..

cmake --build . -j 12 

make install

systemctl start UniversalDevice

systemctl start UniversalDeviceFrontend

cd ..
