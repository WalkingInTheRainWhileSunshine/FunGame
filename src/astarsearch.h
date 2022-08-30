#ifndef ASTARSEARCH_H
#define ASTARSEARCH_H

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "gridstate.h"

using std::cout;
using std::ifstream;
using std::istringstream;
using std::sort;
using std::string;
using std::vector;
using std::abs;

class Astarsearch {
public:
  /**
   * Implementation of A* search algorithm
   */

  vector<vector<GridState>> Search(vector<vector<GridState>> grid, int init[2]);
  int goal[2];

  //directional deltas
  const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

private:
  /**
   * Compare the F values of two cells.
   */
  static bool Compare(const vector<int> a, const vector<int> b);

  /**
   * Sort the two-dimensional vector of ints in descending order.
   */
  void CellSort(vector<vector<int>> *v);

  /**
   * Calculate the manhattan distance
   */
  int Heuristic(int x1, int y1, int x2, int y2);

  /**
   * Check that a cell is valid: on the grid, not an obstacle, and clear.
   */
  bool CheckValidCell(int x, int y, vector<vector<GridState>> &grid);

  /**
   * Add a node to the open list and mark it as open.
   */
  void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &openlist, vector<vector<GridState>> &grid);

  /**
   * Expand current nodes's neighbors and add them to the open list.
   */
  void ExpandNeighbors(const vector<int> &current, int goal[2], vector<vector<int>> &openlist,
                       vector<vector<GridState>> &grid);

  class Node {
  public:
      Node * parent = nullptr;
      double x = 0.f;
      double y = 0.f;
      float h = std::numeric_limits<float>::max();
      float g = 0.0;
      bool visited = false;
      std::vector<Node *> neighbors;

      //void FindNeighbors();
      /*float distance(Node other) const {
        return std::sqrt(std::pow((x - other.x), 2) + std::pow((y - other.y), 2));
      }*/

      Node(){}

  private:
      int index;
      //Node * FindNeighbor(std::vector<int> node_indices);
  };


};


#endif //ASTARSEARCH_H
