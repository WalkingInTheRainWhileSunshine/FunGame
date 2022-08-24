#include <bits/stdc++.h>
#include "game.h"
#include <iostream>
#include <string>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }
std::string Game::GetUserName() {
    std::cout << "Please enter your name \n";
    std::string name;
    std::getline(std::cin, name);
    return name;
}

void Game::SaveScore() const {
    // get name and score
    auto name = GetUserName();
    auto userScore = GetScore();

    // write new score file
    std::ofstream scoreFile;
    scoreFile.open("newSnakeScores.txt", std::ios_base::app);
    bool addUserToFile = true;

    // read old score file to get rank of player
    std::ifstream oldScoreFile;
    oldScoreFile.open("snakeScores.txt");
    if (!oldScoreFile) {
        std::cout << "Congrats to your first game!";
    } else {
        // Check for the place in score list
        std::string line;
        std::vector<int> allScores;
        while ( std::getline(oldScoreFile, line) ){
            auto oldScore = line.substr(line.find(" "));
            // add old score to score list if it is a new player or the old score of a known player is recorded
            if ( (line.find(name) == std::string::npos) || (std::stoi(oldScore) > userScore) ) {
                scoreFile << line << std::endl;
                allScores.emplace_back(std::stoi(oldScore));
            }
            // do not write new score to score list if player already had higher scores
            if ((line.find(name) != std::string::npos) && (std::stoi(oldScore) > userScore))
                addUserToFile = false;
        }
        auto totalPlayers = allScores.size() + 1; // including current player
        sort(allScores.begin(), allScores.end());
        if (allScores.back() < score){
            std::cout << "You are currently No. 1 of " << totalPlayers << std::endl;
        }
        else {
            auto it = std::upper_bound(allScores.begin(), allScores.end(), score);
            auto rank = std::distance( it, allScores.end()) + 1;
            std::cout << "You are currently No. " << rank << " out of " << totalPlayers
                      << std::endl;
            std::cout << "Highest score is " << allScores.back() << std::endl;
        }
    }
    oldScoreFile.close();

    // write new score to file
    if (addUserToFile)
        scoreFile << name << " " << userScore << std::endl;
    scoreFile.close();
    remove("snakeScores.txt");
    rename("newSnakeScores.txt", "snakeScores.txt");
}
