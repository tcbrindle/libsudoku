
cmake_minimum_required(VERSION 3.6)

project(sudoku C CXX)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_EXTENSIONS Off)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")

    set(CMAKE_C_STANDARD 90)
    set(CMAKE_C_EXTENSIONS Off)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Werror")
endif()

add_library(sudoku
    src/c_api.cpp
    src/grid.cpp
    src/solver.cpp
    )
target_include_directories(sudoku PUBLIC include)

add_executable(sudoku-solver src/main.cpp)
target_link_libraries(sudoku-solver sudoku)

enable_testing()

add_subdirectory(doc)
add_subdirectory(example)
add_subdirectory(test)

