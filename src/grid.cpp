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

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/iterator/stream_iterators.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/istream.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>


namespace rng = ranges;

namespace tcb {
namespace sudoku {

static constexpr auto replace_view = [] (char value, char with) {
    return rng::views::transform([value, with] (char c) {
        return c == value ? with : c;
    });
};

auto grid::parse(std::string_view str) -> std::optional<grid>
{
    auto view = rng::views::all(str)
            | rng::views::filter([](char c) { return c == '.' || (c >= '0' && c <= '9'); })
            | replace_view('0', '.')
            | rng::views::take(81);

    if (rng::distance(view) < 81) {
        return std::nullopt;
    }

    auto g = grid{};
    rng::copy(view, rng::begin(g.cells_));
    return g;
}

auto grid::parse(std::istream& istream) -> std::optional<grid>
{
    auto count = 0;
    auto g = grid{};
    auto range = rng::istream_range<char>{istream}
        | rng::views::filter([](char c) { return c == '.' || (c >= '0' && c <= '9'); })
        | replace_view('0', '.')
        | rng::views::take(81);

    // The above range is single-pass, so we cannot call distance() on it
    // with out using it up. Instead, we'll manually copy the characters over
    // and count as we go
    for (char c : range) {
        g.cells_[count] = c;

        if (++count == 81) {
            break;
        }
    }

    if (count < 81) {
        return std::nullopt;
    }

    return g;
}

auto operator<<(std::ostream& os, const grid& g) -> std::ostream&
{
    //   * after every 9th element, insert a '\n'
    //   * for every 3rd and 6th (but not 9th) element, insert a '|'
    //   * after cells 27 and 54, insert "------+------+------\n"
    //   * after every element not mentioned, insert a ' '.
    //
    // This was much more fun with ranges...
    for (std::size_t i = 0; i < g.size(); i++) {
        if (i == 0) {
            // pass
        } else if (i % 27  == 0) {
            os << "\n------+-------+------\n";
        } else if (i % 9 == 0) {
            os << '\n';
        } else if (i % 3  == 0) {
            os << " | ";
        } else {
            os << ' ';
        }
        os << g[i];
    }


    return os;
}

}
}