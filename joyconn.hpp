#pragma once

#include <bluefruit.h>

struct JoyconnButtons
{
  bool top;
  bool joystick;
};

struct JoyconnStick
{
  float x;
  float y;
};

struct JoyconnMotion
{
  float x;
  float y;
  float z;
};

 struct JoyconnData
 {
    JoyconnButtons buttons;
    JoyconnStick joystick;
    JoyconnMotion gyro;
    JoyconnMotion accel;  

    JoyconnData(uint8_t * data, uint16_t len);
    void print();
};

struct JoyconnService
{
  private:
    BLEClientService sensor_service;
    BLEClientCharacteristic sensor_characteristic;
    JoyconnData latest_data;

  public:
    Joyconn();

    void begin();
    
    bool advertiser_is_joyconn(ble_gap_evt_adv_report_t *report);
    void connect(uint16_t conn_handle);
    void disconnect();
    bool is_connected();
    
    JoyconnData read();
    const JoyconnData & get_data();
};

extern JoyconnService Joyconn;
