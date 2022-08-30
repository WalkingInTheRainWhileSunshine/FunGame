#include "snake.h"
#include <cmath>
#include <iostream>

void Snake::Update(std::vector<std::vector<GridState>> grid) {
  // update snakeGrid
  snakeGrid = grid;

  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(
          head_y)};  // We first capture the head's cell before updating.
  UpdateHead();
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell);
  }
}

void Snake::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= speed;
      break;

    case Direction::kDown:
      head_y += speed;
      break;

    case Direction::kLeft:
      head_x -= speed;
      break;

    case Direction::kRight:
      head_x += speed;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);

  // update snake grid
  snakeGrid[static_cast<int>(head_x)][static_cast<int>(head_y)] = GridState::kSnake;
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
    // Remove the tail from the snakeGrid
    auto tailX = body.begin()->x;
    auto tailY = body.begin()->y;
    snakeGrid[tailX][tailY] = GridState::kEmpty;
  } else {
    growing = false;
    size++;
  }
  // Add previous head location to the snakeGrid
  snakeGrid[prev_head_cell.x][prev_head_cell.y] = GridState::kSnake;

  // Check if the snake has died.
  for (auto const &item : body) {
    if (current_head_cell.x == item.x && current_head_cell.y == item.y) {
      alive = false;
    }
  }
}

void Snake::GrowBody() { growing = true; }
