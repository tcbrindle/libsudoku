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

#include <stdlib.h>

/*
 * This is a simple example demonstrating the usage of libsudoku in C
 */

int main(void)
{
    SudokuGrid *grid = NULL;
    SudokuGrid *solution = NULL;
    char *formatted = NULL;

    static const char puzzle[] = ""
            "6 . 2 | . 5 . | . . .\n"
            ". . . | . . 3 | . 4 .\n"
            ". . . | . . . | . . .\n"
            "------+-------+------\n"
            "4 3 . | . . 8 | . . .\n"
            ". 1 . | . . . | 2 . .\n"
            ". . . | . . . | 7 . .\n"
            "------+-------+------\n"
            "5 . . | 2 7 . | . . .\n"
            ". . . | . . . | . 8 1\n"
            ". . . | 6 . . | . . .";

    /* Try to parse the above sudoku puzzle, returning a SudokoGrid */
    grid = sudoku_grid_parse(puzzle);

    /* Was parsing successful? */
    if (!grid) {
        fprintf(stderr, "Could not parse grid\n");
        return EXIT_FAILURE;
    }

    /* Yes it was, so let's pretty-print the grid... */
    sudoku_grid_pretty_print(grid);

    /* ...or print a compact representation using sudoku_grid_to_string() */
    printf("\n%s\n\n", sudoku_grid_to_string(grid));

    /* We can also use sudoku_grid_to_string() to access the grid's values. For
     * example, what does the 33rd cell contain? */
    printf("grid[32] = %c\n\n", sudoku_grid_to_string(grid)[32]);

    /* Of course, the purpose of the library is to solve sudokus, not just print
     * them. To do that, use the sudoku_solve() function: */
    solution = sudoku_solve(grid);

    /* Again, we need to check the return value */
    if (!solution) {
        fprintf(stderr, "Could not solve grid\n");
        return EXIT_FAILURE;
    }

    /* Looks like it worked fine, so let's print the result, this time by
     * obtaining a formatted string first: */
    formatted = sudoku_grid_to_formatted_string(solution);
    printf("Solution:\n%s\n", formatted);
    free(formatted);

    /* Don't forget to free SudokuGrids when you've finished with them */
    sudoku_grid_free(solution);
    sudoku_grid_free(grid);

    return EXIT_SUCCESS;
}
