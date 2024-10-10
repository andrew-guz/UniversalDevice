# where to search headers
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

# unit tests section
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests)

# create output dir because it should exist to copy tests files there
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB_RECURSE TEST_SOURCES *.cpp)
foreach(TEST_TARGET ${TEST_SOURCES})
  get_filename_component(EXECUTABLE_NAME ${TEST_TARGET} NAME_WE)
  add_executable(${EXECUTABLE_NAME} ${TEST_TARGET})
  target_link_libraries(${EXECUTABLE_NAME} Catch2WithMain UniversalDeviceLib
                        UniversalDeviceServiceLib)
  add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
endforeach()

# copy files needed by tests
file(GLOB_RECURSE TEST_JSONS *.json)
foreach(TEST_TARGET ${TEST_JSONS})
  configure_file(${TEST_TARGET} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
  configure_file(${TEST_TARGET} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()
file(GLOB_RECURSE TEST_LOGS *.log)
foreach(TEST_TARGET ${TEST_LOGS})
  configure_file(${TEST_TARGET} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
  configure_file(${TEST_TARGET} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()
