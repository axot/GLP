include("${PROJECT_SOURCE_DIR}/Common.cmake")

add_executable(SparsePlsTest SparsePlsTest.cpp)
target_link_libraries(SparsePlsTest glp)

