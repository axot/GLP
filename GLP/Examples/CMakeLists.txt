add_executable(gspls gspls.cpp)
add_executable(gspan gspan.cpp)
add_executable(gspls_static gspls.cpp)
add_executable(gspan_static gspan.cpp)

target_link_libraries(gspls glp)
target_link_libraries(gspan glp)
target_link_libraries(gspls_static glp_static)
target_link_libraries(gspan_static glp_static)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(gspls_static PROPERTIES LINK_FLAGS "-static")
  set_target_properties(gspan_static PROPERTIES LINK_FLAGS "-static")
ENDIF()

install_targets(/bin gspls)
install_targets(/bin gspan)
install_targets(/bin gspls_static)
install_targets(/bin gspan_static)
