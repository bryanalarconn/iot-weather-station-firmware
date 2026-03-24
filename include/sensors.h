#pragma once

#include <Arduino.h>

struct WeatherData {
    float tempC;
    float tempF;
    float humidity;
    float pressureHPa;
    float heatIndexF;
    int   lightPct;
    bool  rainLikely;
    bool  thresholdExceeded;
    float thresholdF;
};

void sensors_init();
void sensors_read(WeatherData &data);
void sensors_print(const WeatherData &data);
