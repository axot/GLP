project(GLP)
cmake_minimum_required(VERSION 2.6)

# Build Type
if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
endif()

# Output Dictionary
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../../Release/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../../Release/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../../Release)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../../Debug/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../../Debug/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../../Debug)

if(CMAKE_BUILD_TYPE MATCHES Release)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})        
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})        
endif()
 
# Compiler && flags
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS "-Wall -Wno-deprecated -fPIC -fno-strict-aliasing -funroll-all-loops -fopenmp")
set(CMAKE_CXX_LDFLAGS "-shared")

# Check library dependency
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.40.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
find_package(Eigen3 3.1.2 REQUIRED)
if(EIGEN3_FOUND)
  include_directories(${EIGEN3_INCLUDE_DIR})
endif()

include_directories(${CMAKE_SOURCE_DIR}/../build/Headers)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# The version number.
set(GLP_VERSION_MAJOR 1 CACHE INTERNAL "")
set(GLP_VERSION_MINOR 0 CACHE INTERNAL "")

add_subdirectory(Classes)
add_subdirectory(Examples)

enable_testing()
add_subdirectory(Test)
