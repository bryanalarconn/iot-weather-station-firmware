/*
 * ============================================================
 *  WEATHER MONITORING STATION
 *  Board    : Inland ESP32 DevKit
 *  Framework: Arduino (PlatformIO)
 * ============================================================
 *
 *  WIRING REFERENCE
 *  ----------------
 *  DHT11 Temperature & Humidity Sensor:
 *    VCC  → 3.3V
 *    GND  → GND
 *    DATA → GPIO4   (add 10kΩ pull-up resistor to 3.3V)
 *
 *  BMP180 Barometric Pressure Sensor (I2C):
 *    VCC → 3.3V
 *    GND → GND
 *    SDA → GPIO21
 *    SCL → GPIO22
 *    Note: I2C address is fixed at 0x77
 *
 *  Photoresistor — voltage divider (pull-down config):
 *    3.3V → [Photoresistor] → GPIO34 → [10kΩ] → GND
 *    (Brighter light → higher ADC reading)
 *
 *  Potentiometer (high-temp alert threshold):
 *    Left  pin → GND
 *    Wiper pin → GPIO35
 *    Right pin → 3.3V
 *    (Wiper voltage linearly maps to 50–100 °F alert threshold)
 * ============================================================
 */

#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "network.h"

static WeatherData    data;
static unsigned long  lastReadMs = 0;

void setup() {
    Serial.begin(115200);
    sensors_init();
    wifi_connect();
    Serial.println("[INFO] Weather station ready.");
}

void loop() {
    unsigned long now = millis();
    if (now - lastReadMs >= READ_INTERVAL_MS) {
        lastReadMs = now;
        sensors_read(data);
        sensors_print(data);
        http_post(data);
    }
}
