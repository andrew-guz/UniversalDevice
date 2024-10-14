cmake_files=`find . -name CMakeLists.txt -not -path "*/vendor/*"`

FILES=$(find ./src -name "*.*pp")
echo $FILES

/usr/bin/clang-format -i --verbose $FILES && /usr/bin/cmake-format -l error -i $cmake_files

./.vscode/notify.sh $?

