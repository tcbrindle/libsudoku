/**
@file
@author Tristan Brindle
@copyright (c) 2017 Tristan Brindle <tcbrindle@gmail.com>
*/
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

/// @defgroup cpp_api libsudoku C++ API
/// @{
/// @example example.cpp

#ifndef TCB_SUDOKU_HPP_INCLUDED
#define TCB_SUDOKU_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <functional>
#include <iosfwd>
#include <optional>
#include <string_view>



namespace tcb {
namespace sudoku {

/// A class representing a sudoku grid.
/// A grid always contains exactly 81 elements, where each element is a character
/// in the range `[1-9]`, or the character `.`. Grids are immutable once
/// created.
class grid {
    // 81 squares plus a null terminator for easy conversion to a C string
    using cells_type = std::array<char, 82>;
public:
    /// @cond
    using value_type = typename cells_type::value_type;
    using reference = typename cells_type ::reference ;
    using const_reference = typename cells_type::const_reference;
    using pointer = typename cells_type::pointer;
    using const_pointer = typename cells_type::const_pointer;
    using iterator = typename cells_type::const_iterator;
    using const_iterator = typename cells_type::const_iterator;
    using difference_type = typename cells_type::difference_type;
    using size_type = typename cells_type::size_type;
    /// @endcond

    /// Parse a string to create a new grid.
    /// All characters other than `[0-9]` and `.` are ignored. A `0` is
    /// interpreted as a `.`, that is, an unknown value.
    ///
    /// Will fail (returning `nullopt`) if fewer than 81 valid characters could
    /// be read.
    static auto parse(std::string_view str) -> std::optional<grid>;

    /// Parse a stream to create a new grid.
    /// All characters other than `[0-9]` and `.` are ignored. A `0` is
    /// interpreted as a `.`, that is, an unknown value.
    ///
    /// Will fail (returning `nullopt`) if fewer than 81 valid characters could
    /// be read (i.e. if the end-of-stream was reached).
    static auto parse(std::istream& istream) -> std::optional<grid>;

    /// Default constructs an empty grid of 81 '.'s.
    grid() { cells_.fill('.'); cells_.back() = '\0'; }

    /// Returns a random-access const iterator to the start of the cell range.
    auto begin() const -> const_iterator { return std::begin(cells_); }
    /// Returns a random-access const iterator to the start of the cell range.
    auto cbegin() const -> const_iterator { return std::cbegin(cells_); }
    /// Returns a random-access const iterator to the end of the cell range.
    auto end() const -> const_iterator { return std::prev(std::end(cells_)); }
    /// Returns a random-access const iterator to the end of the cell range.
    auto cend() const -> const_iterator { return std::prev(std::cend(cells_)); }

    /// Swaps the contents of this grid with that of `other`
    void swap(grid& other) { cells_.swap(other.cells_); }

    /// Returns the size of the grid, i.e. 81
    auto size() const -> size_type { return 81; }

    /// Returns the maximum size of a grid, i.e. 81
    auto max_size() const -> size_type { return 81; }

    /// Returns whether the grid is empty; this is always `false`
    auto empty() const -> bool { return false; }

    /// Returns the first element of the grid
    auto front() const -> const_reference { return cells_.front(); }

    /// Returns the last element of the grid
    auto back() const -> const_reference { return cells_[80]; }

    /// Returns the element at position `idx`
    auto operator[](size_type idx) const -> const_reference { return cells_[idx]; }

    /// Returns the element at position `idx`, with bounds checking
    auto at(size_type idx) const -> const_reference
    {
        // This is a hack to ensure we get the right behaviour when compiled with -fno-exceptions
        return cells_.at(idx == 81 ? idx + 1: idx);
    }

    /// Returns a const pointer to the raw character array in the grid
    auto data() const -> const_pointer { return cells_.data(); }

private:
    cells_type cells_{};
};

/// Returns `true` if two grids are equal.
/// @relates grid
inline bool operator==(const grid& lhs, const grid& rhs)
{
    return std::equal(std::begin(lhs), std::end(lhs),
                      std::begin(rhs), std::end(rhs));
}

/// Returns `true` if two grids are different.
/// @sa operator==(const grid&, const grid&)
/// @relates grid
inline bool operator!=(const grid& lhs, const grid& rhs)
{
    return !(lhs == rhs);
}

/// Returns `true` if a grid is "less than" another.
/// One grid is considered "less than" another if it has more unknown cells.
/// If two grids have the same number of unknown cells then the cells are
/// compared lexicographically.
/// @relates grid
inline bool operator<(const grid& lhs, const grid& rhs)
{
    const auto a = std::count(std::cbegin(lhs), std::cend(lhs), '.');
    const auto b = std::count(std::cbegin(rhs), std::cend(rhs), '.');
    if (a == b) {
        return std::lexicographical_compare(std::cbegin(lhs), std::cend(lhs),
                                            std::cbegin(rhs), std::cend(rhs));
    }
    return a > b;
}

/// Returns `true` if a grid is "greater than" another
/// @sa operator<(const grid&, const grid&)
/// @relates grid
inline bool operator>(const grid& lhs, const grid& rhs)
{
    return rhs < lhs;
}

/// @returns `true` if a grid is "less than or equal to" another
/// @sa operator<(const grid&, const grid&)
/// @relates grid
inline bool operator<=(const grid& lhs, const grid& rhs)
{
    return !(rhs < lhs);
}

/// Returns `true` if a grid is "greater than or equal to" another
/// @sa operator<(const grid&, const grid&)
/// @relates grid
inline bool operator>=(const grid& lhs, const grid& rhs)
{
    return !(lhs < rhs);
}

/// Swaps the contents of `lhs` and `rhs`
/// @sa tcb::sudoku::grid::swap()
/// @relates grid
inline void swap(grid& lhs, grid& rhs)
{
    lhs.swap(rhs);
}

/// Pretty-prints a grid as a recognisable sudoku board.
std::ostream& operator<<(std::ostream& os, const grid& g);

/// Attempts to solve the given grid.
/// If the solve algorithm fails or the supplied grid contains no solutions,
/// returns `nullopt`. Otherwise returns the new, completed grid.
auto solve(const grid& grid_) -> std::optional<grid>;

/// Returns a string (well, `string_view`) representation of the given grid
inline auto to_string(const grid& grid) -> std::string_view
{
    return {grid.data(), 81};
}

} // end namespace sudoku
} // end namespace tcb


namespace std {

/// Standard hash specialisation for grids.
/// Allows a `grid` to be placed in a `std::unordered_set`, or used as the key
/// in a `std::unordered_map`.
template <>
struct hash<tcb::sudoku::grid>
{
    /// @cond
    using argument_type = tcb::sudoku::grid;
    using result_type = size_t;

    size_t operator()(const tcb::sudoku::grid& grid) const
    {
        return hash<string_view>{}(to_string(grid));
    }
    /// @endcond
};
} // end namespace std

#endif
