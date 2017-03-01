
#include <tcb/sudoku.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int i;
    for (i = 1; i < argc - 1; i += 2) {
        const char *file_path = argv[i];
        int num_puzzles = atoi(argv[i + 1]);
        int n_parsed = 0;
        FILE* stream = NULL;
        SudokuGrid *grid = NULL;

        stream = fopen(file_path, "r");
        if (!stream) {
            fprintf(stderr, "Error opening file %s: %s\n", file_path, strerror(errno));
            return EXIT_FAILURE;
        }

        while ((grid = sudoku_grid_fscan(stream))) {
            SudokuGrid *soln = NULL;

            soln = sudoku_solve(grid);
            if (!soln) {
                fprintf(stderr, "Error solving grid:\n");
                sudoku_grid_pretty_fprint(stderr, grid);
                sudoku_grid_free(grid);
                return EXIT_FAILURE;
            }

            ++n_parsed;

            sudoku_grid_free(soln);
            sudoku_grid_free(grid);
            grid = NULL;
        }

        if (n_parsed != num_puzzles) {
            fprintf(stderr, "Error: could not parse all puzzles\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
