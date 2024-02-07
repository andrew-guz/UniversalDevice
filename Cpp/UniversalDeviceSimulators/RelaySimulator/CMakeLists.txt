#minimum version
cmake_minimum_required(VERSION 3.15)

# common variable for project name and executable name
set(PROJECT_NAME "RelaySimulator")

#build in Debug mode
set(CMAKE_BUILD_TYPE Debug)

#name of a project
project(${PROJECT_NAME})

#where to search headers
include_directories(./include)
include_directories(./../SimulatorLib/include)
include_directories(./../../UniversalDeviceLib/include)
include_directories(./../../UniversalDeviceLib/include/authentication)
include_directories(./../../UniversalDeviceLib/include/component)
include_directories(./../../UniversalDeviceLib/include/core)
include_directories(./../../UniversalDeviceLib/include/events)
include_directories(./../../UniversalDeviceLib/include/interfaces)
include_directories(./../../UniversalDeviceLib/include/messages)
include_directories(./../../UniversalDeviceLib/include/network)
include_directories(./../../UniversalDeviceLib/include/structures)
include_directories(./../../UniversalDeviceLib/include/structures/commands)
include_directories(./../../UniversalDeviceLib/include/structures/inform)
include_directories(./../../UniversalDeviceLib/include/structures/settings)

#set variable where to put ouput file
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../bin)

#where to search for libraries
link_directories(/usr/local/library)
link_directories(./../../bin/)

#set compiler flags
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall")

#set for linker path where libraries are
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-Wl,-rpath -Wl,/usr/local/lib")

#set C++ minimum version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#this will search for *.cpp in source recursively
file(GLOB_RECURSE SRC_FILES ./source/*.cpp)

#create executable from source
add_executable(${PROJECT_NAME} main.cpp ${SRC_FILES})

#executable needs libraries - so link to installed lisbs
#target_link_libraries(${PROJECT_NAME} -lpthread UniversalDeviceLib SimulatorLib)
target_link_libraries(${PROJECT_NAME} -lpthread UniversalDeviceLib)