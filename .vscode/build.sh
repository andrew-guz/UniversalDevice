mkdir -p build && cd build && cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .. && cmake --build . -j 10

# ../.vscode/notify.sh $?
