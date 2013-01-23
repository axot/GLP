include("${PROJECT_SOURCE_DIR}/Common.cmake")

file(GLOB_RECURSE SOURCES "*.cpp")

add_library(glp SHARED ${SOURCES})
set_target_properties(glp PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(glp PROPERTIES VERSION ${GLP_VERSION_MAJOR}.${GLP_VERSION_MINOR} SOVERSION ${GLP_VERSION_MAJOR})

add_library(glp_static STATIC ${SOURCES})
set_target_properties(glp_static PROPERTIES OUTPUT_NAME "glp")
set_target_properties(glp_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

add_custom_command(TARGET glp
                   POST_BUILD
                   COMMAND mkdir -p "${CMAKE_SOURCE_DIR}/../build/Headers/GLP"
                   COMMAND rm -rf "${CMAKE_SOURCE_DIR}/../build/Headers/GLP"
                   COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/../build/Headers/GLP"
                   COMMAND find "${CMAKE_SOURCE_DIR}/../build/Headers/GLP/" -type f | grep -E -v \"\\.h\" | xargs rm)

install_targets(/lib glp glp_static)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION include/GLP
        FILES_MATCHING PATTERN "*.h")
