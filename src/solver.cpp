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

#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#ifdef _MSC_VER
#include <intrin.h> 
#endif


namespace tcb {
namespace sudoku {

namespace rng = ranges;

namespace {

auto enumerate()
{
    return rng::make_pipeable([](auto&& range) {
        using range_t = decltype(range);
        return rng::views::zip(rng::views::iota(0), std::forward<range_t>(range));
    });
}

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
#ifdef _MSC_VER
        return __popcnt(bits);
#else
        return __builtin_popcount(bits);
#endif
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
    auto not_value = [value] (int i) { return i != value; };
    return rng::all_of(rng::views::iota(1, 10) | rng::views::filter(not_value),
                       [&] (int i) { return eliminate(p, index, i); });
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
        const auto& peers = get_peers(index);
        if (!rng::all_of(peers, [&](auto peer) { return eliminate(p, peer, d); })) {
            return false;
        }
    }

    // If a unit u is reduced to only one place for a value, then put it there.
    const auto& units = { get_row(index), get_column(index), get_box(index) };
    return rng::all_of(units, [&] (const auto& u) {
        auto places = rng::views::all(u) | rng::views::filter([&](auto idx) {
            return p[idx].could_be(value);
        });

        const auto size = rng::distance(places);

        if (size == 0) {
            return false;
        }
        if (size == 1) {
            return assign(p, rng::front(places), value);
        }
        return true;
    });
}

auto grid_to_puzzle(const grid& g) -> std::optional<puzzle_t>
{
    auto puzzle = puzzle_t{};
    auto view = rng::views::all(g)
                | enumerate()
                | rng::views::filter([] (const auto& pair) {
        return pair.second != '.';
    });
    if (rng::all_of(view, [&] (const auto& pair) {
        return assign(puzzle, pair.first, pair.second - '0');
    })) {
        return puzzle;
    }
    return std::nullopt;
}

auto puzzle_to_grid(const puzzle_t& p) -> grid
{
    std::array<char, 81> array;
    rng::transform(p, rng::begin(array), [&] (const cell_t& c) {
        if (c.count() > 1) {
            return '.';
        } else {
            return static_cast<char>(c.get_value() + '0');
        }
    });

    return std::move(*grid::parse({array.data(), 81}));
}

auto do_solve(const puzzle_t& p) -> std::optional<puzzle_t>
{
    // If all cells have only one possibility, we're done
    if (rng::all_of(p, [](const auto& c) { return c.count() == 1; })) {
        return std::move(p);
    }

    // Otherwise, make a list of indices that have more than one possibility
    auto non_fixed_cells = rng::views::iota(0, 81) | rng::views::filter([&](int i) {
        return p[i].count() != 1;
    });

    // Choose one of the elements with the fewest possibilities
    auto min_idx = rng::min(non_fixed_cells, [&](int idx1, int idx2) {
        return p[idx1].count() < p[idx2].count();
    });

    // Now try each value in the range [1, 9] in the cell at min_idx
    auto maybe_solutions = rng::views::iota(1, 10)
            | rng::views::filter([&](int i) { return p[min_idx].could_be(i); })
            | rng::views::transform([&] (int i) {
                // Take a new copy of the puzzle and try to assign i to the chosen index
                auto p_copy = p;
                // if the assignment succeeded (generated no contradictions),
                // recursively try to solve the new puzzle
                if (assign(p_copy, min_idx, i)) {
                    return do_solve(p_copy);
                }
                return std::optional<puzzle_t>{};
            });

    // FIXME: We should be able to filter the above view with view::filter(),
    // and then just use `if (begin(solns) != end(solns)) { return front(solns) }`
    // here. But that seems to be very much slower than using a for loop, even
    // though it should be equivalent.
    for (auto&& opt : maybe_solutions) {
        if (opt) {
            return std::move(opt);
        }
    }
    return std::nullopt;
}

}

auto solve(const grid& g) -> std::optional<grid>
{
    auto puzzle = grid_to_puzzle(g);
    if (!puzzle) {
        return std::nullopt;
    }
    auto result = do_solve(*puzzle);
    if (result) {
        return puzzle_to_grid(*result);
    }
    return std::nullopt;
}

}}