# minimum version
cmake_minimum_required(VERSION 3.18)

# common variable for project name and executable name
set(PROJECT_NAME "UniversalDevice")

# name of a project
project(${PROJECT_NAME} VERSION 1.15.7)

# create file with version defines
configure_file(Version.hpp.in Version.hpp)

# include build folder so code can find Version.h
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# allow unit tests
enable_testing()

# subprojects
add_subdirectory(vendor)
add_subdirectory(src)

# cpack
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Universal devices on Arduino that can send information to server or be controlled by server."
)
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION})

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "andrew.guz.spb@gmail.com")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS on)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

# This must always be after all CPACK_* variables are defined
include(CPack)
