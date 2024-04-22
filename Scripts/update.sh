#!/bin/bash

clean=${1:-true}

systemctl stop UniversalDeviceFrontend

systemctl stop UniversalDevice

git fetch --all

git pull

git submodule update --init

if $clean; then
    rm -rf build
fi

mkdir -p build 

cd build 

cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17 --fresh ..

cmake --build . -j 12 

make install

systemctl start UniversalDevice

systemctl start UniversalDeviceFrontend

cd ..
