#include "test.hpp"
#include "pancake/core/spatial.hpp"

using Pancake::SpatialHashGrid;

TEST("elements are found in every cell their bounds overlap") {
    SpatialHashGrid<int> grid(4);
    grid.add(1, glm::vec2(-1.0f, -1.0f), glm::vec2(5.0f, 1.0f)); // Cells x -1..1, y -1..0
    CHECK(grid.get(-1, -1).count(1) == 1);
    CHECK(grid.get(1, 0).count(1) == 1);
    CHECK(grid.get(2, 0).count(1) == 0);
}

TEST("updating an element moves it between cells") {
    SpatialHashGrid<int> grid(4);
    grid.add(1, 1.0f, 1.0f, 1.0f, 1.0f);
    grid.update(1, 41.0f, 41.0f, 1.0f, 1.0f);
    CHECK(grid.get(0, 0).empty());
    CHECK(grid.get(10, 10).count(1) == 1);
}

TEST("removing an element clears it from all cells") {
    SpatialHashGrid<int> grid(4);
    grid.add(1, 0.0f, 0.0f, 10.0f, 10.0f);
    grid.add(2, 0.0f, 0.0f, 1.0f, 1.0f);
    grid.remove(1);
    CHECK(grid.get(0.0f, 0.0f, 20.0f, 20.0f).size() == 1);
    CHECK(grid.get(0.0f, 0.0f, 20.0f, 20.0f).count(2) == 1);
    grid.remove(1); // Removing twice is harmless.
}

TEST("region queries return each element once") {
    SpatialHashGrid<int> grid(1);
    grid.add(7, 0.0f, 0.0f, 10.0f, 10.0f);
    CHECK(grid.get(0.0f, 0.0f, 10.0f, 10.0f).size() == 1);
}

TEST("iteration visits every occupied cell") {
    SpatialHashGrid<int> grid(4);
    grid.add(1, 2.0f, 2.0f, 1.0f, 1.0f);     // Entirely within cell (0, 0).
    grid.add(2, 102.0f, 102.0f, 1.0f, 1.0f); // Entirely within cell (25, 25).
    int cells = 0;
    for (std::vector<int>& elements : grid) {cells++; CHECK(elements.size() == 1);}
    CHECK(cells == 2);
}

RUN_TESTS()
