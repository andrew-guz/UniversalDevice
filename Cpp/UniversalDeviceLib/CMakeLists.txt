#minimum version
cmake_minimum_required(VERSION 3.15)

# common variable for project name and executable name
set(PROJECT_NAME "UniversalDeviceLib")

#check that all needed librarties are preinstalled
function (check_library lib_name)
    find_library(${lib_name}_LIB ${lib_name})
    if(NOT ${lib_name}_LIB)
        message(FATAL_ERROR "${lib_name} library was not found")
    endif()
endfunction()
check_library(uuid)
check_library(curlpp)
check_library(b64)

#build in Debug mode
set(CMAKE_BUILD_TYPE Debug)

#name of a project
project(${PROJECT_NAME})

#where to search headers
include_directories(./include)
include_directories(./include/authentication)
include_directories(./include/component)
include_directories(./include/core)
include_directories(./include/events)
include_directories(./include/interfaces)
include_directories(./include/messages)
include_directories(./include/network)
include_directories(./include/structures)
include_directories(./include/structures/commands)
include_directories(./include/structures/inform)
include_directories(./include/structures/settings)

#set variable where to put ouput file
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin)

#where to search for libraries
link_directories(/usr/local/library)

#set compiler flags
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall")

#set for linker path where libraries are
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-Wl,-rpath -Wl,/usr/local/lib")

#set C++ minimum version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#this will search for *.cpp in source recursively
file(GLOB_RECURSE SRC_FILES ./source/*.cpp)

#create shared library from source
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

#executable needs libraries - so link to installed libs
target_link_libraries(${PROJECT_NAME} -luuid -lcurlpp -lb64)

#unit tests section
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests)

file(GLOB TEST_SOURCES tests/*.cpp)
foreach(TEST_TARGET ${TEST_SOURCES})
    get_filename_component(EXECUTABLE_NAME ${TEST_TARGET} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${TEST_TARGET})
    target_link_libraries(${EXECUTABLE_NAME} Catch2WithMain UniversalDeviceLib)
    add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
endforeach()

#install section
install(TARGETS ${PROJECT_NAME})