#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "astarsearch.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetAStarSearchScore() const;
  int GetSize() const;
  static std::string GetUserName();
  void SaveScore(std::string name, int score) const;

 private:
  bool running {true};
  std::vector<std::vector<GridState>> grid {};

  Snake snake;
  Snake astarSnake;
  SDL_Point food;
  std::vector<SDL_Point> obstacle;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int playerScore {0};
  int computerScore {0};
  int addObstacle {0};

  void InitializeGrid(std::size_t grid_width, std::size_t grid_height);
  void PlaceFood();
  void PlaceObstacle();
  void Update();
  void UpdatePlayerSnake();
  void UpdateAstarSearchSnake();
  bool CheckCell(int x, int y);

  Astarsearch SearchAlgo {};

  string CellString(GridState cell) {
      switch(cell) {
        case GridState::kObstacle: return " O ";
        case GridState::kPath: return " P ";
        case GridState::kSnake: return " s ";
        case GridState::kStart: return " * ";
        case GridState::kFinish: return " F ";
        default: return " 0 ";
      }
    }


};

#endif