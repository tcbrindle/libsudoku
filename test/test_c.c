
#include <tcb/sudoku.h>

#include <assert.h>
#include <string.h>

static const char puzzle[] =   "6..3.2....4.....1..........7.26............543.........8.15........4.2........7..";
static const char solution[] = "615382479943765812827491536752634198168279354394518627286157943579843261431926785";

int main()
{
    SudokuGrid *grid = NULL;
    SudokuGrid *solved = NULL;

    grid = sudoku_grid_parse(puzzle);
    assert(grid);
    assert(strcmp(puzzle, sudoku_grid_to_string(grid)) == 0);

    sudoku_grid_pretty_fprint(stdout, grid);

    solved = sudoku_solve(grid);
    assert(solved);
    assert(strcmp(solution, sudoku_grid_to_string(solved)) == 0);
    (void) solution;

    sudoku_grid_pretty_print(solved);


    sudoku_grid_free(solved);
    sudoku_grid_free(grid);

    return 0;
}
