
#include <tcb/sudoku.h>

/* Make sure asserts fire, even in release builds */
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <string.h>

static const char solvable[] = "6.2.5.........3.4..........43...8....1....2........7..5..27...........81...6.....";
static const char solvable_soln[] = "682154379951763842374892165437528916816937254295416738568271493729345681143689527";
static const char empty[] = ".................................................................................";
static const char empty_soln[] = "123456789456789123789123456231674895875912364694538217317265948542897631968341572";
/*static const char unsolvable[] = ".....5.8....6.1.43..........1.5........1.6...3.......553.....61........4.........";*/
static const char unsolvable[] = "111111111........................................................................";

static void test_parse_from_string()
{
    SudokuGrid *grid = NULL;

    grid = sudoku_grid_parse(solvable);
    assert(grid != NULL);
    sudoku_grid_free(grid);

    grid = sudoku_grid_parse("Some nonsense");
    assert(grid == NULL);
}

static void test_string_conversion()
{
    SudokuGrid *grid = NULL;
    const char* out = NULL;

    grid = sudoku_grid_parse(solvable);
    out = sudoku_grid_to_string(grid);

    assert(strcmp(out, solvable) == 0);
    sudoku_grid_free(grid);
}

static void test_simple_solve()
{
    SudokuGrid *grid = NULL;
    SudokuGrid *soln = NULL;

    grid = sudoku_grid_parse(solvable);
    soln = sudoku_solve(grid);

    assert(soln);
    assert(strcmp(solvable_soln, sudoku_grid_to_string(soln)) == 0);

    sudoku_grid_free(grid);
    sudoku_grid_free(soln);
}

static void test_empty_solve()
{
    SudokuGrid *grid = NULL;
    SudokuGrid *soln = NULL;

    grid = sudoku_grid_parse(empty);
    soln = sudoku_solve(grid);

    assert(soln);
    assert(strcmp(empty_soln, sudoku_grid_to_string(soln)) == 0);

    sudoku_grid_free(grid);
    sudoku_grid_free(soln);
}

static void test_unsolvable()
{
    SudokuGrid *grid = NULL;
    SudokuGrid *soln = NULL;

    grid = sudoku_grid_parse(unsolvable);
    assert(grid);
    soln = sudoku_solve(grid);

    assert(!soln);
    sudoku_grid_free(grid);
}

static void test_null()
{
    SudokuGrid *grid = NULL;
    const char* str = NULL;

    /* Ensure all API calls handle being passed nullptr */
    grid = sudoku_grid_parse(NULL);
    assert(!grid);

    sudoku_grid_free(NULL);

    sudoku_grid_pretty_print(NULL);
    sudoku_grid_pretty_fprint(NULL, NULL);

    grid = sudoku_grid_fscan(NULL);
    assert(!grid);

    str = sudoku_grid_to_string(NULL);
    assert(!str);

    grid = sudoku_solve(NULL);
    assert(!grid);
}

int main()
{
    test_parse_from_string();
    test_string_conversion();
    test_simple_solve();
    test_empty_solve();
    test_unsolvable();
    test_null();

    return 0;
}
