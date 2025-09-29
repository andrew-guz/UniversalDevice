# common variable for project name and executable name
set(PROJECT_NAME "UniversalDeviceLib")

# check that all needed librarties are preinstalled
function(check_library lib_name)
  find_library(${lib_name}_LIB ${lib_name})
  if(NOT ${lib_name}_LIB)
    message(FATAL_ERROR "${lib_name} library was not found")
  endif()
endfunction()
check_library(uuid)

# build in Debug mode
set(CMAKE_BUILD_TYPE Debug)

# name of a project
project(${PROJECT_NAME})

# where to search headers
include_directories(./authentication)
include_directories(./component)
include_directories(./cache)
include_directories(./core)
include_directories(./events)
include_directories(./interfaces)
include_directories(./marshaling)
include_directories(./messages)
include_directories(./network)
include_directories(./structures)
include_directories(./structures/commands)
include_directories(./structures/inform)
include_directories(./structures/settings)
include_directories(./utils)

# set variable where to put ouput file
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../../bin)

# where to search for libraries
link_directories(/usr/local/library)

# set compiler flags
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall")

# set for linker path where libraries are
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS}
                           "-Wl,-rpath -Wl,/usr/local/lib")

# set C++ minimum version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set warnings as errors
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# this will search for *.cpp in source recursively
file(GLOB_RECURSE SRC_FILES *.cpp)

# create shared library from source
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

# executable needs libraries - so link to installed libs
target_link_libraries(${PROJECT_NAME} b64 ixwebsocket fmt::fmt
                      nlohmann_json::nlohmann_json -luuid)

# install section
install(TARGETS ${PROJECT_NAME})
