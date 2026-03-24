#pragma once

#include "sensors.h"

void wifi_connect();
void http_post(const WeatherData &data);
