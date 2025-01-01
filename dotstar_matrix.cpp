#include "dotstar_matrix.hpp"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStar.h>
#include <Fonts/TomThumb.h>

#define WIDTH 12
#define HEIGHT 6

#define DATAPIN    11
#define CLOCKPIN   13

#define SHIFTDELAY 100
#define BRIGHTNESS 20

DisplayService Display;

DisplayService::DisplayService() : Adafruit_DotStarMatrix(
                                  WIDTH, HEIGHT, DATAPIN, CLOCKPIN,
                                  DS_MATRIX_BOTTOM + DS_MATRIX_LEFT +
                                  DS_MATRIX_ROWS + DS_MATRIX_PROGRESSIVE,
                                  DOTSTAR_BGR)
{

}

void DisplayService::begin()
{
  const uint16_t primaryColors[] = {
    Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255)
  };

  Adafruit_DotStarMatrix::begin();
  setFont(&TomThumb);
  setTextWrap(false);
  setBrightness(BRIGHTNESS);

  for (byte i = 0; i < 3; i++) {
    fillScreen(primaryColors[i]);
    show();
    delay(500);
  }
}

void DisplayService::scrolling_print_reset()
{
    scrolling_print_x = width();
}

void DisplayService::scrolling_print(const char * string)
{ 
  static const uint16_t adaColors[] = {
    Color(255, 0, 0),   //A red
    Color(255, 125, 0), //D orange
    Color(200, 255, 0), //A yellowish
    Color(0, 255, 0),   //F green
    Color(0, 255, 225), //R blue
    Color(150, 0, 255), //U purple
    Color(255, 0, 220), //I pink
    Color(255, 65, 0),  //T reddish
    Color(255, 220, 0)  //! orange/yellow
  };
  static const int num_colors = sizeof(adaColors) / sizeof(adaColors[0]);

  if (last_string != string)
  {
    scrolling_print_reset();
  }
  last_string = string;

  fillScreen(0);
  setCursor(scrolling_print_x, 5);
  for (byte i = 0; i < strlen(string); i++) {
    // set the color
    setTextColor(adaColors[i % num_colors]);
    // write the letter
    Adafruit_DotStarMatrix::print(string[i]);
  }

  if (--scrolling_print_x < -50) {
    scrolling_print_x = width();
  }
}


Coordinate DisplayService::random_point()
{
  return {static_cast<uint16_t>(random(0, width())), static_cast<uint16_t>(random(0, height()))};
}

void DisplayService::wait()
{
  delay(SHIFTDELAY);
}