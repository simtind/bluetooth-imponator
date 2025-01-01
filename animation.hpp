#pragma once

#include "dotstar_matrix.hpp"
#include "joyconn.hpp"

struct Animation 
{
    void begin(DisplayService & display);
    uint32_t run(DisplayService & display, const JoyconnData & input);

  private:
    uint32_t offset = 0;
    size_t animation_index = 0; 
};
