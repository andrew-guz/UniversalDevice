# common variable for project name and executable name
set(PROJECT_NAME "UniversalDeviceService")

# check that all needed librarties are preinstalled
function(check_library lib_name)
  find_library(${lib_name}_LIB ${lib_name})
  if(NOT ${lib_name}_LIB)
    message(FATAL_ERROR "${lib_name} library was not found")
  endif()
endfunction()
check_library(ssl)
check_library(crypto)

# build in Debug mode
set(CMAKE_BUILD_TYPE Debug)

# name of a project
project(${PROJECT_NAME})

# where to search headers
include_directories(./settings)
include_directories(./../UniversalDeviceLib/authentication)
include_directories(./../UniversalDeviceLib/component)
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
include_directories(./../UniversalDeviceServiceLib/interfaces)
include_directories(./../UniversalDeviceServiceLib/database)
include_directories(./../UniversalDeviceServiceLib/processors)
include_directories(./../UniversalDeviceServiceLib/services)
include_directories(./../UniversalDeviceServiceLib/utils)

# set variable where to put ouput file
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../../bin)

# where to search for libraries
link_directories(/usr/local/library)
link_directories(./../bin/)

# set compiler flags
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -DCROW_ENABLE_SSL")

# set for linker path where libraries are
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS}
                           "-Wl,-rpath -Wl,/usr/local/lib")

# set C++ minimum version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# this will search for *.cpp in source recursively
file(GLOB_RECURSE SRC_FILES *.cpp)

# create executable from source
add_executable(${PROJECT_NAME} main.cpp ${SRC_FILES})

# make sure that this project will be build after some targets
add_dependencies(${PROJECT_NAME} UniversalDeviceLib UniversalDeviceServiceLib)

# executable needs libraries - so link to installed libs
target_link_libraries(${PROJECT_NAME} -lssl -lcrypto Crow::Crow
                      UniversalDeviceLib UniversalDeviceServiceLib)

# install section
set(CMAKE_INSTALL_PREFIX /usr/local/bin/UniversalDevice)
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin/UniversalDevice)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../../bin/${PROJECT_NAME}.json
        DESTINATION /usr/local/bin/UniversalDevice)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../../bin/HOW_TO_RUN.md
        DESTINATION /usr/local/bin/UniversalDevice)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../../bin/authentication.json
        DESTINATION /usr/local/bin/UniversalDevice)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../../bin/ssl
        DESTINATION /usr/local/bin/UniversalDevice)
