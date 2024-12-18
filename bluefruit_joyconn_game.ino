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

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10); // for nrf52840 with native usb

    Serial.println("Bluefruit52 Central Scan Example");
    Serial.println("--------------------------------\n");

    // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
    // SRAM usage required by SoftDevice will increase dramatically with number of connections
    Bluefruit.begin(0, 1);
    Bluefruit.setTxPower(4); // Check bluefruit.h for supported values
    Bluefruit.setName("Bluefruit52");

    // Start Central Scan
    Bluefruit.setConnLedInterval(250);
    Bluefruit.Scanner.setRxCallback(scan_report_callback);
    Bluefruit.Central.setConnectCallback(connected_callback);

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

void joyconn_disconnected_callback(uint16_t conn_handle, uint8_t reason)
{
    Serial.printf("Disconnected, reason 0x%02x\n", reason);
    Joyconn.disconnect();
}

void loop()
{
    while (true)
    {
        if (Joyconn.is_connected())
        {
            Joyconn.read().print();
        }

        delay(100);
    }
}
