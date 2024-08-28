/usr/bin/clang-format-17 --dry-run --verbose Universal*/**/*.cpp Universal*/**/*.hpp && 

exit_code_clang_format=$?

cmake_files=`find . -name CMakeLists.txt -not -path "*/vendor/*"`

/usr/bin/cmake-format --check -l error $cmake_files

exit_code_cmake_format=$?

if [ $exit_code_clang_format = "1" ]; then
    exit 1
elif [ $exit_code_cmake_format = "1" ]; then
    exit 1
fi

exit 0
