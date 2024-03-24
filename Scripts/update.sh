#!/bin/bash

systemctl stop UniversalDeviceFrontend

systemctl stop UniversalDevice

cd ..

git fetch --all

git pull

cd Cpp

make clean

cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17 --fresh . 

cmake --build . -j 12 

ctest

make install

systemctl start UniversalDevice

systemctl start UniversalDeviceFrontend

cd ../Scripts
