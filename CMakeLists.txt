
cmake_minimum_required(VERSION 3.6)

project(sudoku C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${sudoku_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS Off)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS Off)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic")


# FIXME
find_package(Range-v3 REQUIRED)
include_directories(${RANGE_V3_INCLUDE_DIRS})

include_directories(include)

add_library(sudoku
    src/c_api.cpp
    src/grid.cpp
    src/solver.cpp
    )

add_executable(sudoku-solver src/main.cpp)
target_link_libraries(sudoku-solver sudoku)

add_subdirectory(test)

