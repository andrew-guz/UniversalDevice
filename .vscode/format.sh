cmake_files=`find . -name CMakeLists.txt -not -path "*/vendor/*"`

/usr/bin/clang-format-17 -i --verbose src/**/*.cpp src/**/*.hpp && /usr/bin/cmake-format -l error -i $cmake_files

./.vscode/notify.sh $?

