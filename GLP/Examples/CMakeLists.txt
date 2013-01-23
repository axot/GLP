include("${PROJECT_SOURCE_DIR}/Common.cmake")

add_executable(gspls gspls.cpp)
add_executable(gspls_static gspls.cpp)

target_link_libraries(gspls glp)
target_link_libraries(gspls_static glp_static)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(gspls_static PROPERTIES LINK_FLAGS "-static")
ENDIF()

install_targets(/bin gspls)
install_targets(/bin gspls_static)
