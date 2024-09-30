#!/bin/bash

cat CMakeLists.txt | grep ' VERSION ' | awk '/VERSION/{print $3}' | awk '{split($0,a,")"); print a[1]}'
