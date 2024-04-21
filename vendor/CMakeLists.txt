#subprojects

set(CROW_FEATURES "ssl")
add_subdirectory(crow)

set(ENABLE_QT6 off)
set(BUILD_EXAMPLES off)
set(ENABLE_LIBWTTEST off)
set(ENABLE_LIBWTDBO off)
add_subdirectory(wt)

set(USE_TLS on)
add_subdirectory(ixwebsocket)
