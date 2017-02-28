
#include <tcb/sudoku.hpp>

#include <fstream>
#include <iostream>
#include <vector>

struct test_file {
    const char* path;
    int num_puzzles;
};

int main(int argc, char** argv)
{
    for (int i = 1; i < argc - 1; i += 2) {
        const test_file file{argv[i], std::atoi(argv[i + 1])};

        std::ifstream is{file.path};
        if (!is) {
            std::cerr << "Error: could not open test file " << file.path << std::endl;
            return 1;
        }

        int n_parsed = 0;
        auto grid = tcb::sudoku::optional<tcb::sudoku::grid>{};
        while ((grid = tcb::sudoku::grid::parse(is))) {
            ++n_parsed;
            const auto soln = tcb::sudoku::solve(*grid);
            if (!soln) {
                std::cerr << "Error: could not solve grid\n" << *grid
                          << std::endl;
                return 1;
            }
        }
        if (n_parsed != file.num_puzzles) {
            std::cerr << "Error: could not read all puzzles\n";
            return 1;
        }
    }
}