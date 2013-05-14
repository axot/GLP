file(GLOB_RECURSE SOURCES "*.cpp")

add_library(glp SHARED ${SOURCES})
set_target_properties(glp PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(glp PROPERTIES VERSION ${GLP_VERSION_MAJOR}.${GLP_VERSION_MINOR} SOVERSION ${GLP_VERSION_MAJOR})

add_library(glp_static STATIC ${SOURCES})
set_target_properties(glp_static PROPERTIES OUTPUT_NAME "glp")
set_target_properties(glp_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

add_custom_command(TARGET glp
                   PRE_BUILD
                   COMMAND sh "${PROJECT_SOURCE_DIR}/headfiles.sh"
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

install_targets(/lib glp glp_static)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION include/GLP
        FILES_MATCHING PATTERN "*.h")
