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
#include "tables.hpp"


namespace tcb {
namespace sudoku {

using std::experimental::optional;
using std::experimental::nullopt;

namespace {

const auto& get_peers(int index)
{
    return peers_indices[index];
}

const auto& get_row(int index)
{
    return row_indices[index];
}

const auto& get_column(int index)
{
    return column_indices[index];
}

const auto& get_box(int index)
{
    return box_indices[index];
}

struct cell_t {
    cell_t() : bits(0b111'111'111) {}

    bool could_be(int i) const { return (bits & (1 << (i - 1))) != 0; }

    void remove(int i) { bits &= ~(1 << (i - 1)); }

    auto count() const
    {
        return __builtin_popcount(bits);
    }

    int get_value() const
    {
        if (count() == 1) {
            // There are probably smarter ways to do this, but this will do
            for (int i = 1; i < 10; i++) {
                if (could_be(i)) {
                    return i;
                }
            }
        }
        return 0;
    }

private:
    std::uint_least16_t bits : 9;
};

using puzzle_t = std::array<cell_t, 81>;

auto eliminate(puzzle_t& p, int index, int value) -> bool;

auto assign(puzzle_t& p, int index, int value) -> bool
{
    for (int i = 0; i < 10; i++) {
        if (i == value) {
            continue;
        }

        if (!eliminate(p, index, i)) {
            return false;
        }
    }

    return true;
}

auto eliminate(puzzle_t& p, int index, int value) -> bool
{
    auto& cell = p[index];

    if (!cell.could_be(value)) {
        // already eliminated
        return true;
    }

    // If a square s is reduced to one value d, then eliminate d from the peers.
    cell.remove(value);
    if (cell.count() == 0) {
        // removed last value
        return false;
    }
    if (cell.count() == 1) {
        const auto d = cell.get_value();
        for (int peer : get_peers(index)) {
            if (!eliminate(p, peer, d)) {
                return false;
            }
        }
    }

    // If a unit u is reduced to only one place for a value, then put it there.
    const auto& units = { get_row(index), get_column(index), get_box(index) };
    for (const auto& unit : units) {
        int num_places = 0;
        int first_place = -1;
        for (int place : unit) {
            if (p[place].could_be(value)) {
                num_places++;
                if (first_place == -1) {
                    first_place = place;
                }
            }
        }
        if (num_places == 0) {
            return false;
        }
        if (num_places == 1) {
            if (!assign(p, first_place, value)) {
                return false;
            }
        }
    }

    return true;
}

auto grid_to_puzzle(const grid& g) -> optional<puzzle_t>
{
    auto puzzle = puzzle_t{};
    for (int i = 0; i < 81; i++) {
        char c = *(g.begin() + i);
        if (c != '.') {
            if (!assign(puzzle, i, c - '0')) {
                return nullopt;
            }
        }
    }
    return std::move(puzzle);
}

auto puzzle_to_grid(const puzzle_t& p) -> grid
{
    std::array<char, 81> array;
    for (int i = 0; i < 81; i++) {
        const cell_t& c = p[i];
        if (c.count() > 1) {
            array[i] = '.';
        } else {
            array[i] = static_cast<char>(c.get_value() + '0');
        }
    }

    return std::move(*grid::parse({array.data(), 81}));
}

bool is_solved(const puzzle_t& p)
{
    for (const auto& c : p) {
        if (c.count() != 1) {
            return false;
        }
    }
    return true;
}

auto do_solve(const puzzle_t& p) -> optional<puzzle_t>
{
    // If all cells have only one possibility, we're done
    if (is_solved(p)) {
        return std::move(p);
    }


    int min_idx = 0;
    while (p[min_idx].count() == 1) ++min_idx;
    for (int i = 0; i < 81; i++) {
        if (p[i].count() == 1) {
            continue;
        }
        if (p[i].count() < p[min_idx].count()) {
            min_idx = i;
        }
    }


    for (int i = 0; i < 10; i++) {
        if (p[min_idx].could_be(i)) {
            auto p_copy = p;
            if (assign(p_copy, min_idx, i)) {
                auto result = do_solve(p_copy);
                if (result) {
                    return result;
                }
            }
        }
    }

    return nullopt;
}

}

auto solve(const grid& g) -> optional<grid>
{
    auto puzzle = grid_to_puzzle(g);
    if (!puzzle) {
        return nullopt;
    }
    auto result = do_solve(*puzzle);
    if (result) {
        return puzzle_to_grid(*result);
    }
    return nullopt;
}

}}