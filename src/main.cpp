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

#include <tcb/sudoku.hpp>

#include <chrono>
#include <iostream>
#include <iterator>
#include <fstream>

struct timer {
    using clock_type = std::chrono::high_resolution_clock;

    timer() = default;

    template <typename DurationType = std::chrono::microseconds>
    DurationType elapsed() const
    {
        return std::chrono::duration_cast<DurationType>(clock_type::now() - start_);
    }

private:
    clock_type::time_point start_ = clock_type::now();
};

auto solve_one(const tcb::sudoku::grid& grid, bool interactive)
{
    timer t{};
    auto solution = tcb::sudoku::solve(grid);
    auto e = t.elapsed();

    if (interactive) {
        std::cout << grid << "\n\n";

        if (solution) {
            std::cout << *solution << "\n";
        }
        else {
            std::cout << "Could not find solution\n";
        }
        std::cout << e.count() / 1000.00 << "ms elapsed\n";
    }

    return e;
}

auto solve_from_stream(std::istream& stream, bool interactive)
{
    std::string s;
    std::chrono::microseconds total_elapsed{};
    int num_solved = 0;
    while(std::getline(stream, s)) {
        auto grid = tcb::sudoku::grid::parse(s);
        if (!grid) {
            continue;
        }
        total_elapsed += solve_one(*grid, interactive);
        ++num_solved;
    }

    return std::make_pair(num_solved, total_elapsed);
}

int main(int argc, char** argv)
{
    std::chrono::microseconds total_elapsed{};
    int num_solved = 0;

    if (argc == 1) {
        std::tie(num_solved, total_elapsed) = solve_from_stream(std::cin, true);
    } else {
        std::ifstream file{argv[1]};
        std::tie(num_solved, total_elapsed) = solve_from_stream(file, false);
    }

    std::cout << "Solved " << num_solved << " puzzles in " << total_elapsed.count()/1000.0 << "ms\n";
    std::cout << "(Average " << total_elapsed.count()/(1000.0 * num_solved) << "ms per puzzle)\n";
}