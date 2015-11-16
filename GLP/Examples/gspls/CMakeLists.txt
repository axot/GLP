file(GLOB_RECURSE SOURCES "*.cpp" "gspls")

add_executable(gspls ${SOURCES})
add_executable(gspls_static ${SOURCES})

target_link_libraries(gspls LINK_PUBLIC glp)
target_link_libraries(gspls_static LINK_PUBLIC glp_static)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(gspls_static PROPERTIES LINK_FLAGS "-static")
ENDIF()

install_targets(/bin gspls)
install_targets(/bin gspls_static)
