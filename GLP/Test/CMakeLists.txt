add_executable(SparsePlsTest SparsePlsTest.cpp)
target_link_libraries(SparsePlsTest glp)
add_test(SparsePlsTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SparsePlsTest)

add_executable(NipalsTest NipalsTest.cpp)
target_link_libraries(NipalsTest glp)
add_test(SparsePlsTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/NipalsTest)
