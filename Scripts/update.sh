#!/bin/bash

systemctl stop UniversalDeviceFrontend

systemctl stop UniversalDevice

cd ..

git fetch --all

git pull

cd Cpp

make clean

cmake .

make -j 4

make test

make install

systemctl start UniversalDevice

systemctl start UniversalDeviceFrontend

cd ../Scripts

