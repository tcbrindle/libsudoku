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
#include <range/v3/istream_range.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/bounded.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/intersperse.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/replace.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/zip.hpp>


namespace rng = ranges::v3;

namespace tcb {
namespace sudoku {

auto grid::parse(std::string_view str) -> std::optional<grid>
{
    auto view = rng::view::all(str)
            | rng::view::remove_if([](char c) { return c != '.' && (c < '0' || c > '9'); })
            | rng::view::replace('0', '.')
            | rng::view::take(81);

    if (rng::distance(view) < 81) {
        return std::nullopt;
    }

    auto g = grid{};
    rng::copy(view, rng::begin(g.cells_));
    return g;
}

auto grid::parse(std::istream& istream) -> std::optional<grid>
{
    // Annoyingly in this case, but probably with good reason,
    // rng::view::remove_if caches the next value -- which means that in the
    // case of input iterators, it eats up the next token. There may be a way
    // around this but I can't think of one right now, so we'll do things the
    // longwinded way instead
    auto count = 0;
    auto g = grid{};
    auto range = rng::istream_range<char>{istream}
            | rng::view::replace('0', '.')
            | rng::view::bounded;

    for (char c : range) {
        if (c != '.' && (c < '1' || c > '9')) {
            continue;
        }

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
    // And now for some fun. What we'd like to do is this:
    //   * after every 9th element, insert a '\n'
    //   * for every 3rd and 6th (but not 9th) element, insert a '|'
    //   * after cells 27 and 54, insert "------+------+------\n"
    //   * after every element not mentioned, insert a ' '.
    //
    // I'm fully aware that this is a silly way to do it.
    auto range = rng::view::all(g)
            | rng::view::chunk(27)
            | rng::view::transform([] (auto&& third) {
                return rng::view::all(third)
                    | rng::view::chunk(9)
                    | rng::view::transform([](auto&& row) {
                        return rng::view::all(row)
                            | rng::view::chunk(3)
                            | rng::view::join('|')
                            | rng::view::intersperse(' '); })
                    | rng::view::join('\n'); })
            | rng::view::join(std::string_view("\n------+-------+------\n"));

    rng::copy(range, rng::ostream_iterator<>(os));

    return os;
}

}
}