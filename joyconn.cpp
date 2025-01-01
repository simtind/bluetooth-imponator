#include "joyconn.hpp"

#include <array>

JoyconnService Joyconn;

constexpr float mapfloat(long x, long in_min, long in_max, float out_min, float out_max)
{
    return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

constexpr std::array<uint8_t, 16> get_sensor_uuid(uint8_t tail)
{
    return {tail, 0x23, 0x7a, 0xe2, 0x51, 0x6b, 0x55, 0xbb, 0x67, 0x45, 0x6d, 0xf3, 0x6c, 0xe1, 0x75, 0x66};
}

static constexpr auto sensor_service_uuid = get_sensor_uuid(0xe5);
static constexpr auto sensor_characteristic_uuid = get_sensor_uuid(0xe6);

static constexpr float gyro_factor = 0.0001944;
static constexpr float acc_factor = 0.000244;
static constexpr uint16_t frame_length = 17;

static constexpr uint8_t joyconn_x_min = 35;
static constexpr uint8_t joyconn_x_max = 200;
static constexpr uint8_t joyconn_y_min = 50;
static constexpr uint8_t joyconn_y_max = 192;

static constexpr uint32_t pbp_uuid = 0x21c50462;

JoyconnData::JoyconnData(uint8_t *data, uint16_t len)
{
    if (len != frame_length)
    {
        Serial.print("Got unexpected frame length ");
        Serial.println(len);
        return;
    }

    buttons = {
        .top = (data[1] & (1 << 0)) != 0,
        .joystick = (data[1] & (1 << 1)) != 0};

    const auto joystick_x = (uint8_t)(((data[3] & 0x0F) << 4) | ((data[3] & 0xF0) >> 4));
    const auto joystick_y = (uint8_t)data[4];

    joystick = {
        .x = mapfloat(joystick_x, joyconn_x_min, joyconn_x_max, -1.0, 1.0),
        .y = mapfloat(joystick_y, joyconn_y_min, joyconn_y_max, -1.0, 1.0)};

    gyro = {
        .x = ((int16_t)(data[5] | data[6] << 8)) * gyro_factor,
        .y = ((int16_t)(data[7] | data[8] << 8)) * gyro_factor,
        .z = ((int16_t)(data[9] | data[10] << 8)) * gyro_factor};

    accel = {
        .x = ((int16_t)(data[11] | data[12] << 8)) * acc_factor,
        .y = ((int16_t)(data[13] | data[14] << 8)) * acc_factor,
        .z = ((int16_t)(data[15] | data[16] << 8)) * acc_factor};
}

void JoyconnData::print() const 
{
    Serial.println("Buttons:");
    Serial.print("  Top:      ");
    Serial.println(buttons.top);
    Serial.print("  Joystick: ");
    Serial.println(buttons.joystick);

    Serial.println("Joystick:");
    Serial.print("  X: ");
    Serial.println(joystick.x);
    Serial.print("  Y: ");
    Serial.println(joystick.y);

    Serial.println("Gyro:");
    Serial.print("  X: ");
    Serial.println(gyro.x);
    Serial.print("  Y: ");
    Serial.println(gyro.y);
    Serial.print("  Z: ");
    Serial.println(gyro.z);

    Serial.println("Accel:");
    Serial.print("  X: ");
    Serial.println(accel.x);
    Serial.print("  Y: ");
    Serial.println(accel.y);
    Serial.print("  Z: ");
    Serial.println(accel.z);
}

void sensor_notify_callback(BLEClientCharacteristic *chr, uint8_t *data, uint16_t len)
{
    Joyconn.set_data({data, len});
}

JoyconnService::JoyconnService() : sensor_service(sensor_service_uuid.data()), sensor_characteristic(sensor_characteristic_uuid.data())
{
}

void JoyconnService::begin()
{
    sensor_service.begin();

    sensor_characteristic.setNotifyCallback(sensor_notify_callback);
    sensor_characteristic.begin();
}

JoyconnData JoyconnService::read()
{
    uint8_t buffer[frame_length];
    uint16_t length = sensor_characteristic.read(buffer, frame_length);
    return {buffer, length};
}

const JoyconnData & JoyconnService::get_data()
{
    return latest_data;
}

void  JoyconnService::set_data(const JoyconnData & data)
{
    latest_data = data;  
}

bool JoyconnService::is_connected()
{
    return sensor_characteristic.discovered();
}

void JoyconnService::connect(uint16_t conn_handle)
{
    BLEConnection *conn = Bluefruit.Connection(conn_handle);

    Serial.println("Connected");

    Serial.println("Discovering HID Service ... ");

    if (!sensor_service.discover(conn_handle))
    {
        Serial.println("Could not find sensor service");
        conn->disconnect();
    }

    Serial.println("Found sensor service.");

    if (!sensor_characteristic.discover())
    {
        Serial.println("Could not find sensor characteristic");
        conn->disconnect();
    }

    Serial.println("Found sensor characteristic.");

    Serial.println("Enabling notifications");
    if (!sensor_characteristic.writeCCCD(0x0100))
    {
        Serial.println("Could not enable notification");
    }
}


bool JoyconnService::advertiser_is_joyconn(ble_gap_evt_adv_report_t *report)
{
    uint32_t service_uuid;
    if (!Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_SERVICE_DATA_32BIT_UUID, (uint8_t *)&service_uuid, sizeof(service_uuid)))
    {
        return false;
    }

    Serial.printf("Got service 0x%08X\n", service_uuid);
    if (service_uuid != pbp_uuid)
    {
        return false;
    }

    return true;
}