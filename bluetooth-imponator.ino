/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include "joyconn.hpp"
#include "snake.hpp"
#include "joyconn_visualizer.hpp"
#include "dotstar_matrix.hpp"

SnakeGame Snake;
VisualizerGame Visualizer;

enum Effects
{
    visualizer,
    snake,
    num_effects
};

Effects current_effect = visualizer;
bool switching_effect;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10); // for nrf52840 with native usb

    Serial.println("Bluefruit52 Central Scan Example");
    Serial.println("--------------------------------\n");

    Display.begin();

    // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
    // SRAM usage required by SoftDevice will increase dramatically with number of connections
    Bluefruit.begin(0, 1);
    Bluefruit.setTxPower(4); // Check bluefruit.h for supported values
    Bluefruit.setName("Bluefruit52");

    // Start Central Scan
    Bluefruit.setConnLedInterval(250);
    Bluefruit.Scanner.setRxCallback(scan_report_callback);
    Bluefruit.Central.setConnectCallback(connected_callback);
    Bluefruit.Central.setDisconnectCallback(disconnect_callback);

    Bluefruit.Scanner.start(0);

    Joyconn.begin();

    Serial.println("Scanning ...");
}

void scan_report_callback(ble_gap_evt_adv_report_t *report)
{
    if (Joyconn.advertiser_is_joyconn(report))
    {
        Serial.println("Connecting to Joyconn...");
        Bluefruit.Central.connect(report);
        return;
    }

    Bluefruit.Scanner.resume();
}

void connected_callback(uint16_t conn_handle)
{
    Joyconn.connect(conn_handle);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    Serial.printf("Disconnected, reason 0x%02x\n", reason);
}

void begin_effect(Effects effect, DisplayService & display)
{
    switch(effect)
    {
        case Effects::visualizer:
            Visualizer.begin(display);
            break;
        case Effects::snake:
            Snake.begin(display);
            break;
    }
}

uint32_t run_effect(Effects effect, DisplayService & display, const JoyconnData & input)
{
    switch(effect)
    {
        case Effects::visualizer:
            return Visualizer.run(display, input);
        case Effects::snake:
            return Snake.run(display, input);
    }
}

void loop()
{
    static JoyconnButtons buttons = {false, false};
    static uint32_t next_frame = 0;
    if (Joyconn.is_connected())
    {
        const auto joyconn = Joyconn.read();

        if (joyconn.buttons.joystick && joyconn.buttons.top)
        {
            if (!switching_effect)
            {
                switching_effect = true;
                current_effect = static_cast<Effects>((current_effect + 1) % Effects::num_effects);
                begin_effect(current_effect, Display);
            }
        }
        else
        {
            switching_effect = false;
        }
        
        buttons.joystick |= joyconn.buttons.joystick;
        buttons.top |= joyconn.buttons.top;

        if (next_frame >= millis())
        {
            joyconn.buttons.joystick |= buttons.joystick;
            joyconn.buttons.top |= buttons.top;
            buttons = {false, false};
            next_frame = run_effect(current_effect, Display, joyconn);
        }
        else
        {
            delay(10);
        }
    }
    else
    {
      Display.scrolling_print("Not Connected");
      Display.show();
      Display.wait();
    }
}
