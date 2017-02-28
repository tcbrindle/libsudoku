
cmake_minimum_required(VERSION 3.6)

project(sudoku C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${sudoku_SOURCE_DIR}/cmake")

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else()
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_EXTENSIONS Off)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

    set(CMAKE_C_STANDARD 90)
    set(CMAKE_C_EXTENSIONS Off)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic")
endif()

find_package(Range-v3 REQUIRED)

add_library(sudoku
    src/c_api.cpp
    src/grid.cpp
    src/solver.cpp
    )
target_include_directories(sudoku PUBLIC include)
target_include_directories(sudoku PRIVATE ${RANGE_V3_INCLUDE_DIRS})

add_executable(sudoku-solver src/main.cpp)
target_link_libraries(sudoku-solver sudoku)

enable_testing()

add_subdirectory(doc)
add_subdirectory(test)

