file(GLOB_RECURSE TRAIN_SOURCES "gspls-train*")
file(GLOB_RECURSE CLASSIFY_SOURCES "gspls-classify*")

add_executable(gspls-train ${TRAIN_SOURCES})
add_executable(gspls-train-static ${TRAIN_SOURCES})
add_executable(gspls-classify ${CLASSIFY_SOURCES})
add_executable(gspls-classify-static ${CLASSIFY_SOURCES})

target_link_libraries(gspls-train glp)
target_link_libraries(gspls-train-static glp_static)
target_link_libraries(gspls-classify glp)
target_link_libraries(gspls-classify-static glp_static)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(gspls-train-static PROPERTIES LINK_FLAGS "-static")
  set_target_properties(gspls-classify-static PROPERTIES LINK_FLAGS "-static")
ENDIF()

install_programs(/bin gspls)
install_targets(/bin gspls-train)
install_targets(/bin gspls-train-static)
install_targets(/bin gspls-classify)
install_targets(/bin gspls-classify-static)
