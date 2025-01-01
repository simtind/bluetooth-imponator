#include "snake.hpp"
#include <cassert>

void SnakeGame::begin(DisplayService & display)
{
    apple = display.random_point();
    snake_tail.clear();
    snake_direction = Direction::stop;
    snake_head = {0,0};
    running = true;

    snake_color = display.Color(0, 255, 0);
    head_color = display.Color(128, 128, 0);
    apple_color = display.Color(255, 0, 0);
}

uint32_t SnakeGame::run(DisplayService & display, const JoyconnData & input)
{
    const auto next_frame = millis() + frame_rate_ms;
    if (!running)
    {
        if (input.buttons.joystick)
        {
            // Restart game
            begin(display);
        }
        else
        {
            // Display points
            display.scrolling_print(result_string);
            Display.show();
        }

        return next_frame;
    }
    
    // Do movement
    if (input.joystick.x > 0.5 && snake_direction != Direction::left)
    {
        snake_direction = Direction::right;
    }
    else if (input.joystick.x < -0.5 && snake_direction != Direction::right)
    {
        snake_direction = Direction::left;
    }
    else if (input.joystick.y > 0.5 && snake_direction != Direction::down)
    {
        snake_direction = Direction::up;
    }
    else if (input.joystick.y < -0.5 && snake_direction != Direction::up)
    {
        snake_direction = Direction::down;
    }

    switch(snake_direction)
    {
        case Direction::up:
            snake_head.y++;
            break;
        case Direction::down:
            snake_head.y--;
            break;
        case Direction::right:
            snake_head.x++;
            break; 
        case Direction::left:
            snake_head.x--;
            break;
    }

    if (snake_head.x < 0)
    {
      snake_head.x = display.width() - 1;
    }

    if (snake_head.x >= display.width())
    {
      snake_head.x = 0;
    }

    if (snake_head.y < 0)
    {
      snake_head.y = display.height() - 1;
    }

    if (snake_head.y >= display.height())
    {
      snake_head.y = 0;
    }
    
    // Manage apple eating and tail length
    if (snake_direction != Direction::stop)
    {
        if (tail_overlaps(snake_head))
        {
            snprintf(result_string, 15, "%d points", snake_tail.size());
            display.scrolling_print_reset();
            running = false;
        }
        if (snake_head == apple)
        {
            apple = display.random_point();
            while(tail_overlaps(apple) || apple == snake_head)
            {
                apple = display.random_point();
            }
            snake_tail.push_back(snake_tail.back());
        }

        snake_tail.push_front(snake_head);
        snake_tail.pop_back();
    }

    // Draw 
    Display.fillScreen(0);

    for (const auto point : snake_tail)
    {
        Display.drawPixel(point.x,point.y, snake_color);
    }
    
    Display.drawPixel(apple.x,apple.y, apple_color);
    Display.drawPixel(snake_head.x,snake_head.y, head_color);

    Display.show();
    return next_frame;
}

bool SnakeGame::tail_overlaps(const Coordinate & point)
{
    for (const auto tail : snake_tail)
    {
        if (tail == point)
        {
            return true;
        }
    }

    return false;
}

namespace std 
{
    void __attribute__((noreturn)) __throw_length_error(char const* msg)
    {
        Serial.println(msg);
        assert(false);
    }

    void  __attribute__((noreturn)) __throw_bad_alloc(void)
    {
        Serial.println("Bad alloc");
        assert(false);
    }
}