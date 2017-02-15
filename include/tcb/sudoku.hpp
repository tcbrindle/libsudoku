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

#ifndef TCB_SUDOKU_HPP_INCLUDED
#define TCB_SUDOKU_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <experimental/optional>
#include <experimental/string_view>
#include <functional>
#include <iosfwd>

namespace tcb {
namespace sudoku {

/// A class representing a sudoku grid.
/// A grid always contains exactly 81 elements, where each element is a character
/// in the range ['1', '9'], or the character '.'. Grids are immutable once
/// created.
class grid {
public:
    /// Parse a string to create a new grid.
    /// All characters other than ['0'-'9'] and '.' are ignored. A '0' is
    /// interpreted as a '.', that is, an unknown value.
    ///
    /// Will fail (returning `std::experimental::nullopt`) if fewer than 81 valid
    /// characters could be read.
    static auto
    parse(std::experimental::string_view str) -> std::experimental::optional<grid>;

    static auto
    parse(std::istream& istream) -> std::experimental::optional<grid>;

    /// Default constructs an empty grid of 81 '.'s.
    grid() { cells_.fill('.'); cells_.back() = '\0'; }

    auto begin() const { return std::begin(cells_); }
    auto cbegin() const { return std::cbegin(cells_); }

    auto end() const { return std::prev(std::end(cells_)); }
    auto cend() const { return std::prev(std::cend(cells_)); }

private:
    // 81 squares plus a null terminator for easy conversion to a C string
    std::array<char, 82> cells_;
};

/// Returns true if two grids are equal.
inline bool operator==(const grid& lhs, const grid& rhs)
{
    return std::equal(std::begin(lhs), std::end(lhs),
                      std::begin(rhs), std::end(rhs));
}

/// Returns true if two grids are different.
inline bool operator!=(const grid& lhs, const grid& rhs)
{
    return !(lhs == rhs);
}

/// Pretty-prints a grid as a recognisable sudoku board.
std::ostream& operator<<(std::ostream& os, const grid& g);

/// Attempts to solve the given grid.
/// If the solve algorithm fails or the supplied grid contains no solutions,
/// returns `std::experimental::nullopt`. Otherwise returns the new, completed
/// grid.
auto solve(const grid& grid_) -> std::experimental::optional<grid>;

/// Returns a string (well, string_view) representation of @param grid
inline auto to_string(const grid& grid) -> std::experimental::string_view
{
    return {&*grid.begin(), 81};
}

} // end namespace sudoku
} // end namespace tcb


namespace std {

template <>
struct hash<tcb::sudoku::grid>
{
    using argument_type = tcb::sudoku::grid;
    using result_type = size_t;

    size_t operator()(const tcb::sudoku::grid& grid) const
    {
        return hash<experimental::string_view>{}(to_string(grid));
    }
};


} // end namespace std

#endif
