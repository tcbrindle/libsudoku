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

#include <tcb/sudoku.h>
#include <tcb/sudoku.hpp>

#include <cstring>
#include <iostream>
#include <sstream>

struct SudokuGrid {
    tcb::sudoku::grid grid;
};


SudokuGrid* sudoku_grid_parse(const char* str)
{
    if (!str) {
        return nullptr;
    }

    auto gridopt = tcb::sudoku::grid::parse(str);
    if (gridopt) {
        return new (std::nothrow) SudokuGrid{std::move(*gridopt)};
    }
    return nullptr;
}

SudokuGrid* sudoku_grid_scan()
{
    auto gridopt = tcb::sudoku::grid::parse(std::cin);
    if (gridopt) {
        return new (std::nothrow) SudokuGrid{std::move(*gridopt)};
    }
    return nullptr;
}

SudokuGrid* sudoku_grid_fscan(FILE* file)
{
    if (!file) {
        return nullptr;
    }

    // Annoyingly, we need to do all the filtering here, to build up a string
    // to give to grid::parse().
    std::string str;
    static const auto is_ok = [] (char c) {
        return (c >= '0' && c <= '9') || (c == '.');
    };


    while (str.size() < 81) {
        char c;
        if (std::fread(&c, sizeof(char), 1, file) != 1) {
            return nullptr;
        }

        if (!is_ok(c)) {
            continue;
        }

        str.push_back(c);
    }

    auto grid = tcb::sudoku::grid::parse(str);
    if (grid) {
        return new (std::nothrow) SudokuGrid{std::move(*grid)};
    }
    return nullptr;
}

void sudoku_grid_free(SudokuGrid* grid)
{
    delete grid;
}

void sudoku_grid_pretty_print(const SudokuGrid* grid)
{
    if (!grid) {
        return;
    }

    std::cout << grid->grid << std::endl;
}

void sudoku_grid_pretty_fprint(FILE* stream, const SudokuGrid* grid)
{
    if (!stream || !grid) {
        return;
    }

    // This is a bit ugly, but oh well
    std::stringstream s;
    s << grid->grid;

    std::fprintf(stream, "%s\n", s.str().c_str());
}

const char* sudoku_grid_to_string(const SudokuGrid* grid)
{
    if (!grid) {
        return nullptr;
    }
    return grid->grid.data();
}

char* sudoku_grid_to_formatted_string(const SudokuGrid* grid)
{
    if (!grid) {
        return nullptr;
    }

    std::stringstream ss;
    ss << grid->grid;

    return strdup(ss.str().c_str());
}

SudokuGrid* sudoku_solve(const SudokuGrid* grid)
{
    if (!grid) {
        return nullptr;
    }
    auto out = tcb::sudoku::solve(grid->grid);
    if (!out) {
        return nullptr;
    }
    return new (std::nothrow) SudokuGrid{std::move(*out)};
}