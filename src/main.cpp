#include <iostream>
#include "controller.h"
#include "game.h"
#include "renderer.h"

int main() {
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};

  Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
  Controller controller;
  Game game(kGridWidth, kGridHeight);
  game.Run(controller, renderer, kMsPerFrame);
  std::cout << "Game has terminated successfully!\n";
  // computer score
  std::cout << "A*Search Algo Score: " << game.GetAStarSearchScore() << "\n";
  game.SaveScore("A*Search", game.GetAStarSearchScore());
  // player score
  std::cout << "Player Score: " << game.GetScore() << "\n";
  std::cout << "Size: " << game.GetSize() << "\n";
  game.SaveScore(game.GetUserName(), game.GetScore());


  return 0;
}