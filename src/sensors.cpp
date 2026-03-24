#include "sensors.h"
#include "config.h"
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>

static DHT             dht(DHT_PIN, DHT_TYPE);
static Adafruit_BMP085 bmp;

static float   pressureHistory[PRESSURE_HISTORY];
static uint8_t pressureIdx  = 0;
static bool    pressureFull = false;

// ====================================================================
//  INIT
// ====================================================================
void sensors_init() {
    dht.begin();

    if (!bmp.begin()) {
        Serial.println("[WARN] BMP180 not detected — check wiring.");
    }

    memset(pressureHistory, 0, sizeof(pressureHistory));
}

// ====================================================================
//  PRESSURE TREND — ring buffer
// ====================================================================
static bool updatePressureTrend(float hPa) {
    pressureHistory[pressureIdx] = hPa;
    pressureIdx = (pressureIdx + 1) % PRESSURE_HISTORY;
    if (pressureIdx == 0) pressureFull = true;

    if (pressureFull) {
        float oldest = pressureHistory[pressureIdx];
        float newest = pressureHistory[(pressureIdx + PRESSURE_HISTORY - 1) % PRESSURE_HISTORY];
        return (oldest - newest) > RAIN_DROP_HPA;
    }
    return false;
}

// ====================================================================
//  READ ALL SENSORS
// ====================================================================
void sensors_read(WeatherData &data) {
    // --- DHT11 ---
    float h = dht.readHumidity();
    float c = dht.readTemperature();
    if (!isnan(h) && !isnan(c)) {
        data.humidity   = h;
        data.tempC      = c;
        data.tempF      = dht.readTemperature(true);
        data.heatIndexF = dht.computeHeatIndex(data.tempF, data.humidity);
    } else {
        Serial.println("[WARN] DHT11 read failed — retaining previous values.");
    }

    // --- BMP180 ---
    data.pressureHPa = bmp.readPressure() / 100.0f;
    data.rainLikely  = updatePressureTrend(data.pressureHPa);

    // --- Photoresistor ---
    data.lightPct = (int)map(analogRead(PHOTO_PIN), 0, 4095, 0, 100);

    // --- Potentiometer threshold check ---
    int raw = analogRead(POT_PIN);
    data.thresholdF = POT_TEMP_MIN_F +
                      ((float)raw / 4095.0f) * (POT_TEMP_MAX_F - POT_TEMP_MIN_F);
    data.thresholdExceeded = (data.tempF > data.thresholdF);
}

// ====================================================================
//  SERIAL DEBUG OUTPUT
// ====================================================================
void sensors_print(const WeatherData &data) {
    if (data.thresholdExceeded) {
        Serial.printf("[ALERT] Temp %.1fF exceeds threshold %.1fF!\n",
                      data.tempF, data.thresholdF);
    }

    Serial.printf("[Sensors] %.1fF / %.1fC  Hum:%.1f%%  "
                  "Pres:%.1f hPa  Light:%d%%  HI:%.1fF  Rain:%s\n",
                  data.tempF, data.tempC, data.humidity, data.pressureHPa,
                  data.lightPct, data.heatIndexF,
                  data.rainLikely ? "LIKELY" : "no");
}
