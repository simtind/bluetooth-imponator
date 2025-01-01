
#include "joyconn_visualizer.hpp"

void VisualizerGame::begin(DisplayService & display)
{

}

static const uint16_t colors[] = {
    Display.Color(255, 0, 0),   //A red
    Display.Color(255, 125, 0), //D orange
    Display.Color(200, 255, 0), //A yellowish
    Display.Color(0, 255, 0),   //F green
    Display.Color(0, 255, 225), //R blue
    Display.Color(150, 0, 255), //U purple
    Display.Color(255, 0, 220), //I pink
    Display.Color(255, 65, 0),  //T reddish
    Display.Color(255, 220, 0)  //! orange/yellow
};
static const int num_colors = sizeof(colors) / sizeof(colors[0]);
static int color = 0;

uint32_t VisualizerGame::run(DisplayService & display, const JoyconnData & input)
{
    const auto next_frame = millis() + 10;
    input.print();
    if (input.buttons.joystick)
    {
        color += 1;(color + 1) % (sizeof(colors) / sizeof(colors[0])); 
    }
    if (input.buttons.top)
    {
        color -= 1; 
    }

    if (color < 0)
    {
      color = num_colors - 1;
    }
    if (color >= num_colors)
    {
      color = 0;
    }

    Display.fillScreen(0);
    Display.drawPixel(
        ((input.joystick.x + 1.0) / 2.0) * Display.width(),
        ((input.joystick.y + 1.0) / 2.0) * Display.height(), 
        colors[color]);
    Display.show();
    return next_frame;
}
