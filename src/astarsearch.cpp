#include "astarsearch_1.h"

bool Astarsearch::Compare(const vector<int> a, const vector<int> b) {
  int f1 = a[2] + a[3]; // f1 = g1 + h1
  int f2 = b[2] + b[3]; // f2 = g2 + h2
  return f1 > f2;
}

void Astarsearch::CellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), Compare);
}

int Astarsearch::Heuristic(int x1, int y1, int x2, int y2) {
  return abs(x2 - x1) + abs(y2 - y1);
}

bool Astarsearch::CheckValidCell(int x, int y, vector<vector<GridState>> &grid) {
  bool on_grid_x = (x >= 0 && x < grid.size());
  bool on_grid_y = (y >= 0 && y < grid[0].size());
  if (on_grid_x && on_grid_y)
    return grid[x][y] == GridState::kEmpty;
  return false;
}

void Astarsearch::AddToOpen(int x, int y, int g, int h, vector<vector<int>> &openlist, vector<vector<GridState>> &grid) {
  // Add node to open vector, and mark grid cell as closed.
  openlist.push_back(vector<int>{x, y, g, h});
  grid[x][y] = GridState::kClosed;
}

void Astarsearch::ExpandNeighbors(const vector<int> &current, int goal[2], vector<vector<int>> &openlist, vector<vector<GridState>> &grid) {
  // Get current node's data.
  int x = current[0];
  int y = current[1];
  int g = current[2];

  // Loop through current node's potential neighbors.
  for (int i = 0; i < 4; i++) {
    int x2 = x + delta[i][0];
    int y2 = y + delta[i][1];

    // Check that the potential neighbor's x2 and y2 values are on the grid and not closed.
    if (CheckValidCell(x2, y2, grid)) {
      // Increment g value and add neighbor to open list.
      int g2 = g + 1;
      int h2 = Heuristic(x2, y2, goal[0], goal[1]);
      AddToOpen(x2, y2, g2, h2, openlist, grid);
    }
  }
}

vector<vector<GridState>> Astarsearch::Search(vector<vector<GridState>> grid, int init[2]) {
  // Create the vector of open nodes.
  vector<vector<int>> open {};

  // Initialize the starting node.
  int x = init[0];
  int y = init[1];
  int g = 0;
  int h = Heuristic(x, y, goal[0],goal[1]);
  AddToOpen(x, y, g, h, open, grid);


  while (open.size() > 0) {
    // Get the next node
    CellSort(&open);
    auto current = open.back();
    open.pop_back();
    x = current[0];
    y = current[1];
    grid[x][y] = GridState::kPath;

    // Check if we're done.
    if (x == goal[0] && y == goal[1]) {
      // Set the init grid cell to kStart, and
      // set the goal grid cell to kFinish before returning the grid.
      // Set Start to initial coordinates
      grid[init[0]][init[1]] = GridState::kStart;
      grid[x][y] = GridState::kFinish;
      return grid;
    }

    // If we're not done, expand search to current node's neighbors.
    ExpandNeighbors(current, goal, open, grid);
  }

  // We've run out of new nodes to explore and haven't found a path.
  cout << "No path found!" << "\n";

  return std::vector<vector<GridState>> {};
}