#!/bin/bash

clean=${1:-true}

systemctl stop UniversalDeviceFrontend

systemctl stop UniversalDevice

git fetch --all

git pull

mkdir -p build 

cd build 

if $clean; then
    make clean
fi

cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17 --fresh ..

cmake --build . -j 12 

make install

systemctl start UniversalDevice

systemctl start UniversalDeviceFrontend

cd ..
