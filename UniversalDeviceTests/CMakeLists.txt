#where to search headers
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

#unit tests section
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests)

#create output dir because it should exist to copy tests files there
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB_RECURSE TEST_SOURCES *.cpp)
foreach(TEST_TARGET ${TEST_SOURCES})
    get_filename_component(EXECUTABLE_NAME ${TEST_TARGET} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${TEST_TARGET})
    target_link_libraries(${EXECUTABLE_NAME} Catch2WithMain UniversalDeviceLib)
    add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
endforeach()

#copy files needed by tests
file(GLOB_RECURSE TEST_JSONS *.json)
foreach(TEST_TARGET ${TEST_JSONS})
    configure_file(${TEST_TARGET} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
endforeach()
file(GLOB_RECURSE TEST_JSONS *.log)
foreach(TEST_TARGET ${TEST_JSONS})
    configure_file(${TEST_TARGET} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
endforeach()
