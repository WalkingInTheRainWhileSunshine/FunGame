#include <bits/stdc++.h>
#include "game.h"
#include <iostream>
#include <string>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      astarSnake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  InitializeGrid(grid_width, grid_height);
  snake.snakeGrid = grid;
  astarSnake.snakeGrid = grid;
  PlaceFood();
  // ensure first path search (goal != food)
  SearchAlgo.goal[0] = static_cast<int>(food.x) + 1;
  SearchAlgo.goal[1] = static_cast<int>(food.y) + 1;
  PlaceObstacle();
}

void Game::InitializeGrid(std::size_t grid_width, std::size_t grid_height){
  std::vector<GridState> yVector(grid_height);
  std::fill(yVector.begin(), yVector.end(), GridState::kEmpty);
  std::vector<std::vector<GridState>> initGrid (grid_width, yVector);
  grid = initGrid;
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;


  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();

    renderer.Render(snake, astarSnake, food, obstacle);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(playerScore, frame_count);
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

  // update grid
  grid[food.x][food.y] = GridState::kEmpty;

  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by another item before placing
    // food.
    if (CheckCell(x, y)) {
      food.x = x;
      food.y = y;

      // update grid
      grid[food.x][food.y] = GridState::kFood;
      return;
    }
  }
}

void Game::PlaceObstacle() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by another item before placing
    // obstacles.
    if (CheckCell(x, y)) {
      SDL_Point newObstacle;
      newObstacle.x = x;
      newObstacle.y = y;
      obstacle.push_back(newObstacle);

      // update grid
      grid[newObstacle.x][newObstacle.y] = GridState::kObstacle;
      return;
    }
  }
}

bool Game::CheckCell(int x, int y) {

  return (snake.snakeGrid[x][y] == GridState::kEmpty) && (astarSnake.snakeGrid[x][y] == GridState::kEmpty);
}

void Game::Update() {
  UpdatePlayerSnake();
  // update grid by current snake position
  //grid = snake.snakeGrid;
  UpdateAstarSearchSnake();
  // update grid by current snake position
  //grid = astarSnake.snakeGrid;


  // snake: Check if there's food over here
  bool playerSnakeAte {false}, astarSearchSnakeAte {false};
  if (food.x == static_cast<int>(snake.head_x) && food.y == static_cast<int>(snake.head_y)) {
    playerSnakeAte = true;

    playerScore++;
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.01;
    astarSnake.speed += 0.01;
  }
  // astarSnake: Check if there's food over here
  if (food.x == static_cast<int>(astarSnake.head_x) && food.y == static_cast<int>(astarSnake.head_y)) {
    playerSnakeAte = true;

    computerScore++;
    // Grow snake and increase speed.
    astarSnake.GrowBody();
    astarSnake.speed += 0.01;
    snake.speed += 0.01;
  }

  std::min(snake.speed, maxSpeed);
  std::min(astarSnake.speed, maxSpeed);

  if (playerSnakeAte || astarSearchSnakeAte) {
    PlaceFood();
    // Place an obstacle each 5th cycle of new food
    addObstacle += 1;
    if (addObstacle%5 == 0)
      PlaceObstacle();
  }
}

void Game::UpdateAstarSearchSnake(){
  if (!snake.alive || !astarSnake.alive){
    running = false;
    return;
  }

  // search path
  int start[2];
  start[0] = static_cast<int>(astarSnake.head_x);
  start[1] = static_cast<int>(astarSnake.head_y);
  SearchAlgo.goal[0] = static_cast<int>(food.x);
  SearchAlgo.goal[1] = static_cast<int>(food.y);

  auto astarSnakePathGrid = grid;
  astarSnakePathGrid[SearchAlgo.goal[0]][SearchAlgo.goal[1]] = GridState::kEmpty;
  astarSnakePathGrid = SearchAlgo.Search(astarSnakePathGrid, start);
  /* 
  // debug output
  for (int i = 0; i < astarSnakePathGrid.size(); i++) {
    for (int j = 0; j < astarSnakePathGrid[i].size(); j++) {
      std::cout << CellString(astarSnakePathGrid[i][j]);
    }
    cout << "\n";
  }
  cout << "\n \n \n";
  */

  // set input directions according to search path
  for (int i = 0; i < 4; i++) {
    auto maxX = grid.size();
    auto maxY = grid[0].size();

    int x2 = start[0] + SearchAlgo.delta[i][0];
    int y2 = start[1] + SearchAlgo.delta[i][1];


    if (x2 < maxX && y2 < maxY && (astarSnakePathGrid[x2][y2] == GridState::kPath || astarSnakePathGrid[x2][y2] == GridState::kFinish)){
      switch (i) {
        case 0: {
          //if (astarSnake.direction == Snake::Direction::kRight) continue;
          astarSnake.direction = Snake::Direction::kLeft;
          break;
        }
        case 1: {
          //if (astarSnake.direction == Snake::Direction::kDown) continue;
          astarSnake.direction = Snake::Direction::kUp;
          break;
        }
        case 2: {
          //if (astarSnake.direction == Snake::Direction::kLeft) continue;
          astarSnake.direction = Snake::Direction::kRight;
          break;
        }
        case 3: {
          //if (astarSnake.direction == Snake::Direction::kUp) continue;
          astarSnake.direction = Snake::Direction::kDown;
          break;
        }
      }
    }
  }

  astarSnake.Update(grid);
  grid = astarSnake.snakeGrid;
  // Check for collision with obstacles
  if (grid[astarSnake.head_x][astarSnake.head_y] == GridState::kObstacle) {
    astarSnake.alive = false;
    running = false;
    return;
  }
}

void Game::UpdatePlayerSnake(){
  // check for collision of snake head with its body
  if (!snake.alive || !astarSnake.alive) {
    running = false;
    return;
  }
  // update snake according to current user inputs
  snake.Update(grid);
  // Check for collision with obstacles
  if (grid[snake.head_x][snake.head_y] == GridState::kObstacle) {
    snake.alive = false;
    running = false;
    return;
  }
}

int Game::GetAStarSearchScore() const { return computerScore; }
int Game::GetScore() const { return playerScore; }
int Game::GetSize() const { return snake.size; }
std::string Game::GetUserName() {
    std::cout << "Please enter your name \n";
    std::string name;
    std::getline(std::cin, name);
    return name;
}

void Game::SaveScore(std::string name, int userScore) const {

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
        if (allScores.back() < userScore){
            std::cout << "You are currently No. 1 of " << totalPlayers << std::endl;
        }
        else {
            auto it = std::upper_bound(allScores.begin(), allScores.end(), userScore);
            auto rank = std::distance( it, allScores.end()) + 1;
            std::cout << "You are currently No. " << rank << " out of " << totalPlayers
                      << std::endl;
            std::cout << "Highest score is " << allScores.back() << std::endl << std::endl;
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
