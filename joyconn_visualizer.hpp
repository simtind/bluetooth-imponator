#pragma once

#include "joyconn.hpp"
#include "dotstar_matrix.hpp"

struct VisualizerGame 
{
    void begin(DisplayService & display);
    uint32_t run(DisplayService & display, const JoyconnData & input);
};
