
add_executable(test_cpp catch_main.cpp test_cpp.cpp)
target_link_libraries(test_cpp sudoku)

add_executable(test_c test_c.c)
target_link_libraries(test_c sudoku)
