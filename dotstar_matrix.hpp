#pragma once

#include <Adafruit_DotStarMatrix.h>

struct Coordinate
{
  int32_t x;
  int32_t y;

  bool operator==(const Coordinate& rhs) const
  {
     return (x == rhs.x) && (y == rhs.y);
  }
};

struct DisplayService : Adafruit_DotStarMatrix
{
    DisplayService();

    void begin();
    void scrolling_print_reset();
    void scrolling_print(const char * string);

    Coordinate random_point();

    void wait();

    private:
      int scrolling_print_x;
};

extern DisplayService Display;