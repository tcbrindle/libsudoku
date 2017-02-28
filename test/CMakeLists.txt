
add_executable(test_cpp_api catch_main.cpp test_cpp_api.cpp)
target_link_libraries(test_cpp_api sudoku)
add_test(test_cpp_api test_cpp_api)

add_executable(test_c_api test_c_api.c)
target_link_libraries(test_c_api sudoku)
add_test(test_c_api test_c_api)

add_executable(test_solver_cpp test_solver_cpp.cpp)
target_link_libraries(test_solver_cpp sudoku)
add_test(test_solver_cpp test_solver_cpp
    ${CMAKE_CURRENT_LIST_DIR}/files/easy50.txt 50
    ${CMAKE_CURRENT_LIST_DIR}/files/hard.txt 95)

add_executable(test_solver_c test_solver_c.c)
target_link_libraries(test_solver_c sudoku)
add_test(test_solver_c test_solver_c
    ${CMAKE_CURRENT_LIST_DIR}/files/easy50.txt 50
    ${CMAKE_CURRENT_LIST_DIR}/files/hard.txt 95)
