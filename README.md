# libsudoku

libsudoku is a small library for solving sudoku puzzles. It has both C and C++14 interfaces, and is implemented in C++14 using [Range-V3](https://github.com/ericniebler/range-v3/).

## Building ##

### GCC and Clang ###

Building the library requires a C++14 compiler with implementations of `std::experimental::optional` and `std::experimental::string_view`, capable of handling Range-V3 code.

You'll need to set the CMake configuration variable `RANGE_V3_INCLUDE_DIRS` to point to the `include` directory of a Range-V3 checkout (unless you have the headers installed in a system location such as `/usr/include` or `/usr/local/include`, in which case it should be picked up automatically). You can do this by providing the `-D` switch to CMake on the command line, for example

```
$ mkdir build
$ cd build
$ cmake .. -DRANGE_V3_INCLUDE_DIRS=/path/to/range/v3/include
$ cmake --build ..
```

### Visual Studio ###

Building on Windows requires [Visual Studio 2017](https://www.visualstudio.com/vs/visual-studio-2017-rc/). You can use the version of Range-V3 provided by [vcpkg](https://github.com/Microsoft/vcpkg/), though you'll still need to set the `RANGE_V3_INCLUDE_DIRS` CMake variable as specified above.

Note that at present we only build a static library. A shared library option will be enabled once I've figured out the necessary export macros on Windows.

## Usage ##

The library has two very similar APIs, one for C++ and one for C. The C++ API requires either a recent GCC or Clang, or MSVC 2017. The C API should be compatible with everything back to the stone age, although you'll still need to link with the C++ standard library.

### C++ ###

To use the C++ API, `#include <tcb/sudoku.hpp>`.

The C++ API uses a single type, `tcb::sudoku::grid`, representing a (solved or unsolved) 9x9 sudoku board. A `grid` is immutable once created, and otherwise behaves as a regular type (that is, it can be copied, moved etc freely). You can default construct an empty board, which isn't very useful, but otherwise to create one you need to use the `tcb::sudoku::grid::parse()` static method, which has two overloads. The first takes a `string_view` and the second a `std::istream`. In either case, `parse()` will return an `optional<grid>`.

The parser looks for characters in the range `[1-9]`, interpreting them as known squares on the sudoku board. Unknown squares can be represented as `0` or `.`. All other characters are ignored. If 81 valid characters (i.e. a full board) are read successfully, an engaged `optional` containing a `grid` is returned, and otherwise an empty `optional`. **Always check the return value of `parse()`!** Note that the parser doesn't check to ensure that you have provided a valid sudoku puzzle (i.e. there may be contradicting positions in the input), but invalid boards will not be able to be solved.

As mentioned above, `grid`s are regular types, so you can copy them, move them, compare them for equality, sort them and even `std::hash` them. You can also iterate over the 81 contained characters using the normal standard library `begin()` and `end()` functions. 

To convert a `grid` to a string, use the `tcb::sudoku::to_string()` function (which actually returns a `string_view`). You can pretty-print a grid using the normal stream operator; for example

```cpp
// This is on two lines just to stop line wrapping in the README
// You can of course pass the string literal directly to parse()
const auto str = "......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3.";
const auto grid = tcb::sudoku::grid::parse(str);
if (grid) {
    std::cout << grid.value() << '\n';
}
```

prints

```
. . . | . . . | 5 2 .
. 8 . | 4 . . | . . .
. 3 . | . . 9 | . . .
------+-------+------
5 . 1 | . . . | 6 . .
2 . . | 7 . . | . . .
. . . | 3 . . | . . .
------+-------+------
6 . . | . 1 . | . . .
. . . | . . . | 7 . 4
. . . | . . . | . 3 .
```

To print the compact representation, use  `std::cout << to_string(grid)`:

```
......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3.
```

To solve a puzzle, simply use the `tcb::sudoku::solve()` function. If the solve was successful, the result will be an `optional` containing a completed `grid`. If the solver could not find a solution (for example if the input grid contained contradicting positions), an empty `optional` will be returned. For example:

```cpp

auto str = "......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3."s;
auto grid = tcb::sudoku::grid::parse(str);

auto result = tcb::sudoku::solve(*grid);

if (result) {
    std::cout << *result << '\n'
} else {
    std::cout << "Could not find a solution\n";
}
```

### C ###

To use the C API, `#include <tcb/sudoku.h>`.

As in C++, the library uses one basic type, `SudokuGrid`. This is an opaque type which can be passed around by pointer. To create a grid, use `sudoku_grid_parse()`, which takes a null-terminated string containing a puzzle. This will return a `SudokuGrid*` containing the grid, or `NULL` on failure. After use, you need to free the grid with `sudoku_grid_free()`. For example:

```C
const char* puzzle = "......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3.";
SudokuGrid *grid = sudoku_grid_parse(puzzle);
/* ... do something with grid ... */
sudoku_grid_free(grid);
```

You can use `sudoku_grid_pretty_print()` to pretty-print a grid to `stdout`; the format is the same as shown for C++. To pretty-print a grid elsewhere, use `sudoku_grid_pretty_fprint()` and pass a `FILE*`, for example

```C
SudokuGrid *grid = sudoku_grid_parse(...);
FILE *file = fopen("/path/to/file", "w");
sudoku_grid_pretty_fprint(file, grid);
```

A compact representation of the grid can be obtained by using `sudoku_grid_to_string()`. You must not `free()` the resulting string.

To solve a grid, pass it to the `sudoku_solve()` function. If successul, this will return a new `SudokuGrid*` containing the solution, or `NULL` on failure. For example:

```C
SudokuGrid *grid = NULL, *solution = NULL;

const char* puzzle = "......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3.";
grid = sudoku_grid_parse(puzzle);
if (!grid) {
    printf("Error reading grid\n");
    exit(-1);
}

solution = sudoku_solve(grid);
if (solution) {
    sudoku_grid_pretty_print(solution);
} else {
    printf("Could not find a solution\n");

}

sudoku_grid_free(solution);
sudoku_grid_free(grid);
```
## Licence ##

This library is released under the MIT licence. See LICENCE.txt for full details.
