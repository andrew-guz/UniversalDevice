# common variable for project name and executable name
set(PROJECT_NAME "UniversalDeviceServiceLib")

# check that all needed librarties are preinstalled
function(check_library lib_name)
  find_library(${lib_name}_LIB ${lib_name})
  if(NOT ${lib_name}_LIB)
    message(FATAL_ERROR "${lib_name} library was not found")
  endif()
endfunction()
check_library(pthread)
check_library(sqlite3)
check_library(ssl)
check_library(crypto)

# build in Debug mode
set(CMAKE_BUILD_TYPE Debug)

# name of a project
project(${PROJECT_NAME})

# where to search headers
include_directories(./)
include_directories(./application-settings)
include_directories(./controllers)
include_directories(./database)
include_directories(./interfaces)
include_directories(./processors)
include_directories(./services)
include_directories(./utils)
include_directories(./../UniversalDeviceLib/authentication)
include_directories(./../UniversalDeviceLib/component)
include_directories(./../UniversalDeviceLib/cache)
include_directories(./../UniversalDeviceLib/core)
include_directories(./../UniversalDeviceLib/events)
include_directories(./../UniversalDeviceLib/interfaces)
include_directories(./../UniversalDeviceLib/marshaling)
include_directories(./../UniversalDeviceLib/messages)
include_directories(./../UniversalDeviceLib/network)
include_directories(./../UniversalDeviceLib/structures)
include_directories(./../UniversalDeviceLib/structures/commands)
include_directories(./../UniversalDeviceLib/structures/inform)
include_directories(./../UniversalDeviceLib/structures/settings)
include_directories(./../UniversalDeviceLib/utils)

# where to search for libraries
link_directories(/usr/local/library)
link_directories(./../bin/)

# set compiler flags
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
                    "-Wall -Wno-unused-lambda-capture -DCROW_ENABLE_SSL")

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

# make sure that this project will be build after some targets
add_dependencies(${PROJECT_NAME} UniversalDeviceLib)

# executable needs libraries - so link to installed libs
target_link_libraries(${PROJECT_NAME} -lpthread -lsqlite3 Crow::Crow sunset
                      UniversalDeviceLib)

# install section
install(TARGETS ${PROJECT_NAME})
