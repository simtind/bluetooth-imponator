#pragma once

#include <Arduino.h>

#include "joyconn.hpp"
#include "dotstar_matrix.hpp"

#include <deque>

enum class Direction
{
  stop,
  up,
  down,
  left,
  right
};

struct SnakeGame 
{
    Coordinate snake_head;
    std::deque<Coordinate> snake_tail;
    Direction snake_direction;

    char result_string[15];

    Coordinate apple;

    bool running;

    uint16_t snake_color;
    uint16_t head_color;
    uint16_t apple_color;

    static constexpr uint32_t frame_rate_ms = 300; 

    void begin(DisplayService & display);
    uint32_t run(DisplayService & display, const JoyconnData & input);
    void end(DisplayService & display);

  private:
    bool tail_overlaps(const Coordinate & point);
};
