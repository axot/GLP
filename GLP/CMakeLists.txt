project(GLP)
cmake_minimum_required(VERSION 2.6)

include("Common.cmake")

# The version number.
set(GLP_VERSION_MAJOR 1 CACHE INTERNAL "")
set(GLP_VERSION_MINOR 0 CACHE INTERNAL "")

add_subdirectory(Classes)
add_subdirectory(Examples)
add_subdirectory(Test)
