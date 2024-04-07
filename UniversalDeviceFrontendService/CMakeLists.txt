# common variable for project name and executable name
set(PROJECT_NAME "UniversalDeviceFrontendService")

#build in Debug mode
set(CMAKE_BUILD_TYPE Debug)

#name of a project
project(${PROJECT_NAME})

#where to search headers
include_directories(${CMAKE_BINARY_DIR}/vendor/wt)
include_directories(./../vendor/wt)
include_directories(./../vendor/wt/src)
include_directories(./include)
include_directories(./include/interfaces)
include_directories(./include/customWidgets)
include_directories(./include/stackWidgets)
include_directories(./../UniversalDeviceLib/include)
include_directories(./../UniversalDeviceLib/include/authentication)
include_directories(./../UniversalDeviceLib/include/component)
include_directories(./../UniversalDeviceLib/include/core)
include_directories(./../UniversalDeviceLib/include/events)
include_directories(./../UniversalDeviceLib/include/interfaces)
include_directories(./../UniversalDeviceLib/include/messages)
include_directories(./../UniversalDeviceLib/include/network)
include_directories(./../UniversalDeviceLib/include/structures)
include_directories(./../UniversalDeviceLib/include/structures/commands)
include_directories(./../UniversalDeviceLib/include/structures/inform)
include_directories(./../UniversalDeviceLib/include/structures/settings)

#set variable where to put ouput file
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../bin)

#where to search for libraries
link_directories(/usr/local/library)
link_directories(./../../bin/)
link_directories(${CMAKE_BINARY_DIR}/vendor/wt/src/)
link_directories(${CMAKE_BINARY_DIR}/vendor/wt/src/http)

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

#make sure that this project will be build after some targets
add_dependencies(${PROJECT_NAME} wt wthttp UniversalDeviceLib)

#executable needs libraries - so link to installed libs
target_link_libraries(${PROJECT_NAME} -lwt -lwthttp UniversalDeviceLib)

#install section
set(CMAKE_INSTALL_PREFIX /usr/local/bin/UniversalDevice)
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin/UniversalDevice)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../bin/${PROJECT_NAME}.json DESTINATION /usr/local/bin/UniversalDevice)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../bin/authentication.json DESTINATION /usr/local/bin/UniversalDevice)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../bin/ssl DESTINATION /usr/local/bin/UniversalDevice)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../bin/resources DESTINATION /usr/local/bin/UniversalDevice)
