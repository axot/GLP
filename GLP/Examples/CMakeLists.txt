add_executable(gspan gspan.cpp)
#add_executable(gspan_static gspan.cpp)
#add_executable(gspan_pls_static gspan_pls.cpp)

target_link_libraries(gspan glp)
#target_link_libraries(gspan_static glp_static)
#target_link_libraries(gspan_pls_static glp_static)

#IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  #set_target_properties(gspan_static PROPERTIES LINK_FLAGS "-static")
  #set_target_properties(gspan_pls_static PROPERTIES LINK_FLAGS "-static")
#ENDIF()

install(TARGETS gspan
        RUNTIME DESTINATION bin)

add_subdirectory(gspls)
