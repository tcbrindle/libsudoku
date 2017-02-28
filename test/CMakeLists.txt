
add_executable(test_cpp catch_main.cpp test_cpp.cpp)
target_link_libraries(test_cpp sudoku)
add_test(test_cpp_api test_cpp)

add_executable(test_c test_c.c)
target_link_libraries(test_c sudoku)
add_test(test_c_api test_c)

file(COPY files DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
