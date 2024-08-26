mkdir -p build && cd build && cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17 -DCMAKE_EXPORT_COMPILE_COMMANDS=1 --fresh .. && cmake --build . -j 10

../.vscode/notify.sh $?
