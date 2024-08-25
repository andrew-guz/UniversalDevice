#subprojects

add_subdirectory(crow)
set_target_properties(Crow PROPERTIES CROW_FEATURES "ssl")

set(ENABLE_QT5 off)
set(ENABLE_QT6 off)
set(BUILD_EXAMPLES off)
set(ENABLE_LIBWTTEST off)
set(ENABLE_LIBWTDBO off)
add_subdirectory(wt)

add_subdirectory(ixwebsocket)
set_target_properties(ixwebsocket PROPERTIES USE_TLS on)

add_subdirectory(b64)

add_subdirectory(curlpp)

add_subdirectory(nlohmann_json)

add_subdirectory(catch2)

add_subdirectory(fmt)
set_target_properties(fmt PROPERTIES BUILD_SHARED_LIBS on)
