/*
Copyright (c) 2017 Tristan Brindle <tcbrindle@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

//
// This is a simple example demonstrating the usage of libsudoku in C++
//

#include <tcb/sudoku.hpp>

#include <cassert>
#include <iostream>

int main()
{
    constexpr auto& puzzle = R"(
        6 . 2 | . 5 . | . . .
        . . . | . . 3 | . 4 .
        . . . | . . . | . . .
        ------+-------+------
        4 3 . | . . 8 | . . .
        . 1 . | . . . | 2 . .
        . . . | . . . | 7 . .
        ------+-------+------
        5 . . | 2 7 . | . . .
        . . . | . . . | . 8 1
        . . . | 6 . . | . . .)";

    // Try to parse the above Sudoku puzzle, obtaining an optional<grid>:
    const auto maybe_grid = tcb::sudoku::grid::parse(puzzle);

    // Was parsing successful?
    if (!maybe_grid) {
        std::cerr << "Could not parse grid\n";
        return 1;
    }

    // Yes it was, so let's grab a reference to the actual grid
    const auto& grid = *maybe_grid;

    // We can pretty-print it...
    std::cout << grid << "\n\n";

    // ...or print a compact representation using to_string()
    std::cout << tcb::sudoku::to_string(grid) << "\n\n";

    // Just to demonstrate the API, let's count the unknown cells
    std::cout << "Found " << std::count(std::cbegin(grid), std::cend(grid), '.')
              << " unknown squares\n";

    // What does the 33rd cell contain?
    std::cout << "grid[32] = " << grid[32] << '\n';

    // What does the 101st cell contain?
    try {
        const auto value = grid.at(101);
        std::cout << "grid.at(101) = " << value << '\n';
    }
    catch (const std::out_of_range&) {
        // Just kidding, there is no 101st cell -- a sudoku grid is 9 x 9 = 81
        std::cout << "out_of_range exception when calling grid.at(101)\n\n";
    }

    // Of course, the purpose of the library is to solve sudokus. To do that,
    // we simply use the solve() function, returning another optional<grid>
    const auto solution = tcb::sudoku::solve(grid);

    // Again, we need to check the return value...
    if (!solution) {
        std::cerr << "Could not solve grid\n";
        return 1;
    }

    // Looks like it worked fine, let's print the result
    std::cout << "Solution:\n" << *solution << '\n';
}