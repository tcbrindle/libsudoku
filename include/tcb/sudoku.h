/**
@file
@author Tristan Brindle
@copyright (c) 2017 Tristan Brindle <tcbrindle@gmail.com>
*/
/*
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
/**
 * @defgroup c_api libsudoku C API
 * @{
 * @example example.c
 */

#ifndef TCB_SUDOKU_H_INCLUDED
#define TCB_SUDOKU_H_INCLUDED

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Opaque type representing a grid */
typedef struct SudokuGrid SudokuGrid;

/**
 * Parse a null-terminated string to create a new grid.
 * All characters other than `[0-9]` and `.` are ignored. A `0` is interpreted
 * as a `.`, that is, an unknown value.
 *
 * Will fail (returning `NULL`) if fewer than 81 valid characters could be read.
 *
 * You must free the returned grid with sudoku_grid_free().
 */
SudokuGrid *sudoku_grid_parse(const char *str);

/**
 * Parse a grid from `stdin`.
 * All characters other than `[0-9]` and `.` are ignored. A `0` is interpreted
 * as a `.`, that is, an unknown value.
 *
 * Will fail (returning `NULL`) if fewer than 81 valid characters could be read
 * before encountering the end of the stream.
 *
 * You must free the returned grid with sudoku_grid_free().
 */
SudokuGrid *sudoku_grid_scan(void);

/**
 * Parse a grid from a file stream.
 * All characters other than `[0-9]` and `.` are ignored. A `0` is interpreted
 * as a `.`, that is, an unknown value.
 *
 * Will fail (returning `NULL`) if fewer than 81 valid characters could be read
 * before encountering the end of the stream or an IO error.
 *
 * You must free the returned grid with sudoku_grid_free().
 */
SudokuGrid *sudoku_grid_fscan(FILE* file);

/**
 * Frees a grid created with sudoku_grid_parse() or sudoku_grid_solve()
 */
void sudoku_grid_free(SudokuGrid *grid);

/**
 * Pretty-prints a grid to stdout.
 *
 * Equivalent to
 *
 * ```c
 * char *str = sudoku_grid_to_formatted_string(grid);
 * printf("%s", str);
 * free(str);
 * ```
 */
void sudoku_grid_pretty_print(const SudokuGrid *grid);

/**
 * Pretty-prints a grid to stream.
 *
 * Equivalent to
 *
 * ```c
 * char *str = sudoku_grid_to_formatted_string(grid);
 * fprintf(stream, "%s", str);
 * free(str);
 * ```
 */
void sudoku_grid_pretty_fprint(FILE *stream, const SudokuGrid *grid);

/**
 * Returns a string representation of grid. Do not free.
 */
const char *sudoku_grid_to_string(const SudokuGrid *grid);

/**
 * Returns a formatted string representation of the grid.
 * You must free(3) the returned string
 */
char *sudoku_grid_to_formatted_string(const SudokuGrid *grid);

/**
 * Attempts to solve the given grid.
 * If the solve algorithm fails or the supplied grid contains no solutions,
 * returns `NULL`. Otherwise returns the new, completed grid, which must be
 * freed with sudoku_grid_free()
 */
SudokuGrid *sudoku_solve(const SudokuGrid *grid);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
