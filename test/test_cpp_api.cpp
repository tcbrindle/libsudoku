
#include <tcb/sudoku.hpp>

#include "catch.hpp"

#include <map>
#include <sstream>
#include <unordered_map>

constexpr auto& solvable = "6.2.5.........3.4..........43...8....1....2........7..5..27...........81...6.....";
constexpr auto& solvable_soln = "682154379951763842374892165437528916816937254295416738568271493729345681143689527";
constexpr auto& solvable_printed =
        "6 . 2 | . 5 . | . . .\n"
        ". . . | . . 3 | . 4 .\n"
        ". . . | . . . | . . .\n"
        "------+-------+------\n"
        "4 3 . | . . 8 | . . .\n"
        ". 1 . | . . . | 2 . .\n"
        ". . . | . . . | 7 . .\n"
        "------+-------+------\n"
        "5 . . | 2 7 . | . . .\n"
        ". . . | . . . | . 8 1\n"
        ". . . | 6 . . | . . .";

constexpr auto& empty = ".................................................................................";
constexpr auto& empty_soln = "123456789456789123789123456231674895875912364694538217317265948542897631968341572";

//constexpr auto& unsolvable = ".....5.8....6.1.43..........1.5........1.6...3.......553.....61........4.........";
constexpr auto& unsolvable = "111111111........................................................................";

bool equal(tcb::sudoku::string_view sv, const tcb::sudoku::grid& grid)
{
    return std::equal(std::begin(sv), std::end(sv),
                      std::begin(grid), std::end(grid));
}

/*
 * Grid parsing tests
 */

TEST_CASE("Grids can be parsed from strings", "[parse]")
{
    const auto grid = tcb::sudoku::grid::parse(solvable);
    REQUIRE(grid != tcb::sudoku::nullopt);
    REQUIRE(equal(solvable, *grid));
}

TEST_CASE("Grids can be parsed from streams", "[parse]")
{
    std::stringstream ss;
    ss << solvable << "some junk" << solvable;
    // We should be able to parse multiple grids from streams
    {
        const auto grid = tcb::sudoku::grid::parse(ss);
        REQUIRE(grid != tcb::sudoku::nullopt);
        REQUIRE(equal(solvable, *grid));
    }
    {
        const auto grid = tcb::sudoku::grid::parse(ss);
        REQUIRE(grid != tcb::sudoku::nullopt);
        REQUIRE(equal(solvable, *grid));
    }
}

TEST_CASE("Nonsense strings do not get parsed", "[parse]")
{
    const auto grid = tcb::sudoku::grid::parse("Some nonsense");
    REQUIRE_FALSE(grid);
}

TEST_CASE("Nonsense streams do not get parsed", "[parse]")
{
    std::stringstream ss{"Some nonsense"};
    const auto grid = tcb::sudoku::grid::parse(ss);
    REQUIRE_FALSE(grid);
}

TEST_CASE("Short strings do not get parsed", "[parse]")
{
    const auto sv = tcb::sudoku::string_view(solvable).substr(0, 80);
    const auto grid = tcb::sudoku::grid::parse(sv);
    REQUIRE_FALSE(grid);
}

TEST_CASE("Short streams do not get parsed", "[parse]")
{
    std::string str{tcb::sudoku::string_view(solvable).substr(0, 80)};
    std::stringstream ss{str};
    const auto grid = tcb::sudoku::grid::parse(ss);
    REQUIRE_FALSE(grid);
}

TEST_CASE("Zeros get parsed as dots in strings", "[parse]")
{
    auto str = std::string(solvable);
    std::replace(std::begin(str), std::end(str), '.', '0');
    const auto grid = tcb::sudoku::grid::parse(str);
    REQUIRE(grid);
    REQUIRE(equal(solvable, *grid));
}

TEST_CASE("Zeros get parsed as dots in streams", "[parse]")
{
    auto str = std::string(solvable);
    std::replace(std::begin(str), std::end(str), '.', '0');
    std::stringstream ss{str};
    const auto grid = tcb::sudoku::grid::parse(ss);
    REQUIRE(grid);
    REQUIRE(equal(solvable, *grid));
}

/*
 * Grid output tests
 */

TEST_CASE("Grids can be converted to strings", "[grid]")
{
    const auto grid = *tcb::sudoku::grid::parse(solvable);
    std::string str{tcb::sudoku::to_string(grid)};
    REQUIRE(str == solvable);
}

TEST_CASE("Grids can be printed to streams", "[grid]")
{
    std::stringstream ss;
    const auto grid = *tcb::sudoku::grid::parse(solvable);
    ss << grid;
    REQUIRE(ss.str() == solvable_printed);
}

TEST_CASE("Grids can be round-tripped to streams", "[grid]")
{
    const auto grid1 = *tcb::sudoku::grid::parse(solvable);
    std::stringstream ss;
    ss << grid1;
    const auto grid2 = tcb::sudoku::grid::parse(ss);
    REQUIRE(grid2);
    REQUIRE(equal(solvable, *grid2));
}

/*
 * Regular type tests
 */

TEST_CASE("Grids can be default constructed", "[grid]")
{
    const tcb::sudoku::grid g;
    REQUIRE(equal(empty, g));
}

TEST_CASE("Grids can be tested for equality", "[grid]")
{
    const auto grid1 = *tcb::sudoku::grid::parse(solvable);
    const auto grid2 = *tcb::sudoku::grid::parse(solvable);
    REQUIRE(grid1 == grid2);
    REQUIRE(grid2 == grid1);
}

TEST_CASE("Grids can be tested for inequality", "[grid]")
{
    const auto grid1 = *tcb::sudoku::grid::parse(solvable);
    const auto grid2 = tcb::sudoku::grid{};
    REQUIRE(grid1 != grid2);
    REQUIRE(grid2 != grid1);
}

TEST_CASE("Grid relational operators work as expected", "[grid]")
{
    const auto grid1 = tcb::sudoku::grid{};
    const auto grid2 = *tcb::sudoku::grid::parse(solvable);
    const auto grid3 = *tcb::sudoku::grid::parse(solvable); // == grid2
    const auto grid4 = *tcb::sudoku::grid::parse(solvable_soln);

    // less than
    REQUIRE(grid1 < grid2);
    REQUIRE_FALSE(grid2 < grid1);
    REQUIRE_FALSE(grid2 < grid3);
    REQUIRE_FALSE(grid3 < grid2);
    REQUIRE(grid3 < grid4);
    REQUIRE_FALSE(grid4 < grid3);

    // greater than
    REQUIRE_FALSE(grid1 > grid2);
    REQUIRE(grid2 > grid1);
    REQUIRE_FALSE(grid2 > grid3);
    REQUIRE_FALSE(grid3 > grid2);
    REQUIRE_FALSE(grid3 > grid4);
    REQUIRE(grid4 > grid3);

    // less than or equal
    REQUIRE(grid1 <= grid2);
    REQUIRE_FALSE(grid2 <= grid1);
    REQUIRE(grid2 <= grid3);
    REQUIRE(grid3 <= grid2);
    REQUIRE(grid3 <= grid4);
    REQUIRE_FALSE(grid4 <= grid3);

    // greater than or equal
    REQUIRE_FALSE(grid1 >= grid2);
    REQUIRE(grid2 >= grid1);
    REQUIRE(grid2 >= grid3);
    REQUIRE(grid3 >= grid2);
    REQUIRE_FALSE(grid3 >= grid4);
    REQUIRE(grid4 >= grid3);
}

TEST_CASE("Grids can be copy constructed", "[grid]")
{
    const auto grid1 = *tcb::sudoku::grid::parse(solvable);
    const auto grid2{grid1};
    REQUIRE(grid2 == grid1);
}

TEST_CASE("Grids can be copy-assigned", "[grid]")
{
    const auto grid1 = *tcb::sudoku::grid::parse(solvable);
    auto grid2 = tcb::sudoku::grid{};
    REQUIRE(grid1 != grid2);
    grid2 = grid1;
    REQUIRE(grid2 == grid1);
}

TEST_CASE("Grids can be move constructed", "[grid]")
{
    auto grid1 = *tcb::sudoku::grid::parse(solvable);
    auto grid2{std::move(grid1)};
    REQUIRE(equal(solvable, grid2));
}

TEST_CASE("Grids can be move-assigned", "[grid]")
{
    auto grid1 = *tcb::sudoku::grid::parse(solvable);
    auto grid2 = tcb::sudoku::grid{};
    REQUIRE(grid1 != grid2);
    grid2 = std::move(grid1);
    REQUIRE(equal(solvable, grid2));
}

template <typename Map>
void test_map(Map& map)
{
    const auto grid1 = tcb::sudoku::grid{};
    map[grid1] = "one";
    REQUIRE(map.cbegin()->first == grid1);
    REQUIRE(map.cbegin()->second == "one");
    REQUIRE("one" == map[grid1]);

    const auto grid2 = *tcb::sudoku::grid::parse(solvable);
    map[grid2] = "two";
    REQUIRE(map.size() == 2);
    REQUIRE("two" == map[grid2]);

    map[grid1] = "three";
    REQUIRE(map.size() == 2);
    REQUIRE("three" == map[grid1]);
}

TEST_CASE("Grids can be used as keys in a std::map", "[grid]")
{
    std::map<tcb::sudoku::grid, std::string> map;
    test_map(map);
}

TEST_CASE("Grids can be used as keys in a std::unordered_map", "[grid]")
{
    std::unordered_map<tcb::sudoku::grid, std::string> map;
    test_map(map);
}

/*
 * Container concept tests
 */
TEST_CASE("Grids behave roughly like standard containers", "[grid]")
{
    const auto grid = *tcb::sudoku::grid::parse(solvable);
    REQUIRE(grid.size() == 81);
    REQUIRE(grid.max_size() == 81);
    REQUIRE_FALSE(grid.empty());
    REQUIRE(grid.front() == solvable[0]);
    REQUIRE(grid.back() == solvable[80]);
    for (int i = 0; i < 81; i++) {
        REQUIRE(grid[i] == solvable[i]);
    }
    REQUIRE_NOTHROW(grid.at(0) == solvable[0]);
    REQUIRE_THROWS_AS(grid.at(-1), std::out_of_range);
    REQUIRE_THROWS_AS(grid.at(81), std::out_of_range);
    REQUIRE(std::equal(grid.data(), grid.data() + grid.size(),
                       std::begin(solvable), std::end(solvable) - 1));
}

TEST_CASE("Grids are Swappable", "[grid]")
{
    auto grid1 = *tcb::sudoku::grid::parse(solvable);
    auto grid2 = *tcb::sudoku::grid::parse(unsolvable);

    SECTION("...using member swap()")
    {
        grid1.swap(grid2);
    }

    SECTION("...using member swap() again")
    {
        grid2.swap(grid1);
    }

    SECTION("...using nonmember swap() via ADL")
    {
        using std::swap;
        swap(grid1, grid2);
    }

    SECTION("...using nonmember swap via ADL again")
    {
        using std::swap;
        swap(grid2, grid1);
    }

    SECTION("Using explicitly-qualified nonmember swap")
    {
        tcb::sudoku::swap(grid1, grid2);
    }

    SECTION("Using explicitly-qualified nonmember swap again")
    {
        tcb::sudoku::swap(grid2, grid1);
    }

    REQUIRE(equal(unsolvable, grid1));
    REQUIRE(equal(solvable, grid2));
}

/*
 * Solvability tests
 */

TEST_CASE("Basic solving works correctly", "[solve]")
{
    const auto grid = *tcb::sudoku::grid::parse(solvable);
    const auto soln = tcb::sudoku::solve(grid);
    REQUIRE(soln);
    REQUIRE(equal(solvable_soln, *soln));
}

TEST_CASE("Solving empty grids works as expected", "[solve]")
{
    const auto grid = tcb::sudoku::grid{};
    const auto soln = tcb::sudoku::solve(grid);
    REQUIRE(soln);
    REQUIRE(equal(empty_soln, *soln));
}

TEST_CASE("Unsolvable grids are handled correctly", "[solve]")
{
    const auto grid = tcb::sudoku::grid::parse(unsolvable);
    REQUIRE(grid);
    const auto soln = tcb::sudoku::solve(*grid);
    REQUIRE_FALSE(soln);
}
