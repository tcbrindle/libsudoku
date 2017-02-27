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

#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

constexpr char preamble[] = R"(
#ifndef TCB_SUDOKU_TABLES_HPP
#define TCB_SUDOKU_TABLES_HPP

#include <array>

namespace tcb {

)";

constexpr char postamble[] = R"(} // end namespace tcb

#endif
)";

template <class DelimT, class CharT = char, class Traits = std::char_traits<CharT>>
struct ostream_joiner : std::iterator<std::output_iterator_tag,void,void,void,void>
{
    using char_type = CharT;
    using traits = Traits;
    using ostream_type = std::basic_ostream<CharT, Traits>;

    ostream_joiner(ostream_type& stream, const DelimT& delimiter)
        : out_stream(std::addressof(stream)),
          delim(delimiter)
    {}

    ostream_joiner(ostream_type& stream, DelimT&& delimiter)
        : out_stream(std::addressof(stream)),
          delim(std::move(delimiter))
    {}

    template <class T>
    ostream_joiner& operator=(const T& value) {
        if (!first_element)
            *out_stream << delim;
        first_element = false;
        *out_stream << value;
        return *this;
    }

    ostream_joiner& operator*() noexcept { return *this; }
    ostream_joiner& operator++() noexcept { return *this; }
    ostream_joiner& operator++(int) noexcept { return *this; }

private:
    ostream_type* out_stream = nullptr;
    DelimT delim;
    bool first_element = true;
};

template <class CharT, class Traits, class DelimT>
ostream_joiner<std::decay_t<DelimT>, CharT, Traits>
make_ostream_joiner(std::basic_ostream<CharT, Traits>& os, DelimT&& delimiter)
{
    return ostream_joiner<std::decay_t<DelimT>, CharT, Traits>{os,
                std::forward<DelimT>(delimiter)};
}

auto get_row(int i)
{
    std::array<int, 9> output = {};
    int start = 9 * (i/9);
    std::iota(begin(output), end(output), start);
    return output;
}

auto get_column(int i)
{
    std::array<int, 9> output = {};
    const int start = i % 9;
    int idx = 0;
    std::generate(begin(output), end(output), [&] () {
        return start + (idx++ * 9);
    });
    return output;
}

auto get_box(const int cell)
{
    std::array<int, 9> output = {};
    const int rowstart = 3 * (cell/27);
    const int colstart = 3 * ((cell % 9)/3);
    int i = 0;
    int j = 0;

    std::generate(begin(output), end(output), [&] {
        if (j == 3) {
            j = 0;
            ++i;
        }
        return (9 * (rowstart + i)) + colstart + j++;
    });

    return output;
}

auto get_peers(int i)
{
    std::vector<int> output{};

    auto row = get_row(i);
    auto col = get_column(i);
    auto box = get_box(i);

    auto pred = [i] (int j) { return i != j; };

    std::copy_if(begin(row), end(row), std::back_inserter(output), pred);
    std::copy_if(begin(col), end(col), std::back_inserter(output), pred);
    std::copy_if(begin(box), end(box), std::back_inserter(output), pred);

    std::sort(begin(output), end(output));
    output.erase(std::unique(begin(output), end(output)), std::end(output));

    return output;
}

template <class Func>
void print_table(Func f, int size, const char* name)
{
    std::cout << "constexpr std::array<std::array<int, " << size << ">, 81> "
              << name << "_indices = {{\n";

    for (int i = 0; i < 81; i++) {
        std::cout << "    {{ ";
        const auto r = f(i);
        std::copy(begin(r), end(r), make_ostream_joiner(std::cout, ", "));
        std::cout << " }},\n";
    }

    std::cout << "}};\n\n";
}

int main()
{
    std::cout << preamble;

    print_table(get_row, 9, "row");
    print_table(get_column, 9, "column");
    print_table(get_box, 9, "box");
    print_table(get_peers, 20, "peers");

    std::cout << postamble;
}
