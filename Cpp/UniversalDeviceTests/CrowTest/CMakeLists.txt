#minimum version
cmake_minimum_required(VERSION 3.15)

# common variable for project name and executable name
set(PROJECT_NAME "CrowTest")

#build in Debug mode
set(CMAKE_BUILD_TYPE Debug)

#name of a project
project(${PROJECT_NAME})

#where to search headers
include_directories(./include)

#set variable where to put ouput file
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

#where to search for libraries
link_directories(/usr/local/library)

#set for linker path where libraries are 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/usr/local/lib")

#set C++ minimum version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set list of files by wildcard into global variable
file(GLOB SRC_FILES "./source/*.cpp")

#create executable from source
add_executable(${PROJECT_NAME} main.cpp ${SRC_FILES})

#executable needs libraries - so link to installed lisbs
target_link_libraries(${PROJECT_NAME} -lpthread)