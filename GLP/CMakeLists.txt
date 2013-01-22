project(GLP)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(Classes)
add_subdirectory(Examples)
add_subdirectory(Test)

# The version number.
set(GLP_VERSION_MAJOR 1)
set(GLP_VERSION_MINOR 0)

include("Common.cmake")

