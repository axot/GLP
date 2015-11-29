file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp" "gspls")

add_executable(gspls-train ${SOURCES})
add_executable(gspls-train-static ${SOURCES})

target_link_libraries(gspls-train glp)
target_link_libraries(gspls-train-static glp_static)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(gspls-train-static PROPERTIES LINK_FLAGS "-static")
ENDIF()

install_programs(/bin gspls)
install_targets(/bin gspls-train)
install_targets(/bin gspls-train-static)
