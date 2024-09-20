# minimum version
cmake_minimum_required(VERSION 3.18)

# common variable for project name and executable name
set(PROJECT_NAME "UniversalDevice")

# name of a project
project(${PROJECT_NAME} VERSION 1.12.0)

# create file with version defines
configure_file(Version.hpp.in Version.hpp)

# include build folder so code can find Version.h
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# allow unit tests
enable_testing()

# subprojects
add_subdirectory(vendor)
add_subdirectory(UniversalDeviceLib)
add_subdirectory(UniversalDeviceService)
add_subdirectory(UniversalDeviceFrontendService)
add_subdirectory(UniversalDeviceSimulators/SimulatorLib)
add_subdirectory(UniversalDeviceSimulators/ThermometerSimulator)
add_subdirectory(UniversalDeviceSimulators/RelaySimulator)
add_subdirectory(UniversalDeviceTests)
