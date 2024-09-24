# subprojects

set(CROW_FEATURES "ssl")
add_subdirectory(crow)

set(ENABLE_QT5 off)
set(ENABLE_QT6 off)
set(BUILD_EXAMPLES off)
set(ENABLE_LIBWTTEST off)
set(ENABLE_LIBWTDBO off)
set(SHARED_LIBS off)
add_subdirectory(wt)

set(USE_TLS on)
set(BUILD_SHARED_LIBS on)
add_subdirectory(ixwebsocket)

add_subdirectory(b64 EXCLUDE_FROM_ALL)

add_subdirectory(curlpp)

add_subdirectory(nlohmann_json)

add_subdirectory(catch2)

set(BUILD_SHARED_LIBS on)
add_subdirectory(fmt)
