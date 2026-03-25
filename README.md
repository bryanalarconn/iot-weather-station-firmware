# IoT Weather Station

An ESP32-based weather monitoring station that reads temperature, humidity, barometric pressure, and light level — then posts the data to a remote server over HTTPS.

---

## Hardware

| Component | Purpose |
|---|---|
| Inland ESP32 DevKit | Microcontroller |
| DHT11 | Temperature & humidity |
| BMP180 | Barometric pressure |
| Photoresistor | Light level |
| Potentiometer | High-temperature alert threshold |

---

## Wiring

### DHT11 (Temperature & Humidity)
| DHT11 Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| DATA | GPIO 4 |

> Add a 10kΩ pull-up resistor between DATA and 3.3V.

### BMP180 (Barometric Pressure) — I2C
| BMP180 Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

> I2C address is fixed at 0x77.

### Photoresistor (Light Level) — voltage divider
```
3.3V → [Photoresistor] → GPIO 34 → [10kΩ] → GND
```
Higher light = higher ADC reading = higher light percentage.

### Potentiometer (Alert Threshold)
| Pot Pin | ESP32 Pin |
|---|---|
| Left | GND |
| Wiper | GPIO 35 |
| Right | 3.3V |

Turning the pot adjusts the high-temperature alert threshold between 50°F and 100°F.

---

## Setup

### 1. Install PlatformIO
Install the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode) for VS Code, or use the PlatformIO CLI.

### 2. Configure credentials
Open `include/config.h` and fill in your details:

```cpp
#define WIFI_SSID   "your_wifi_name"
#define WIFI_PASS   "your_wifi_password"
#define POST_URL    "https://your-server.com/weather"
```

### 3. Build and flash
```bash
pio run --target upload
```

### 4. Monitor serial output
```bash
pio device monitor --baud 115200
```

---

## How It Works

Every **5 seconds** the ESP32:
1. Reads all sensors
2. Prints a summary line to the serial monitor
3. Fires an alert if the temperature exceeds the pot threshold
4. POSTs a JSON payload to your server over HTTPS

### Example serial output
```
[Sensors] 76.3F / 24.6C  Hum:58.2%  Pres:1014.5 hPa  Light:63%  HI:76.3F  Rain:no
[HTTP] POST https://your-server.com/weather
[HTTP] Payload:
{
  "temp_f": 76.3,
  "temp_c": 24.6,
  "humidity": 58.2,
  "pressure_hpa": 1014.5,
  "heat_index_f": 76.3,
  "light_pct": 63,
  "rain_likely": false,
  "alert_threshold_f": 85.0,
  "threshold_exceeded": false
}
[HTTP] Response: 200
```

### JSON fields explained

| Field | Description |
|---|---|
| `temp_f` / `temp_c` | Temperature in Fahrenheit and Celsius |
| `humidity` | Relative humidity percentage |
| `pressure_hpa` | Barometric pressure in hectopascals |
| `heat_index_f` | Feels-like temperature (NOAA formula, factors in humidity) |
| `light_pct` | Light level 0–100% |
| `rain_likely` | `true` if pressure dropped more than 2 hPa over the last 10 readings |
| `alert_threshold_f` | Current pot threshold in °F (range: 50–100°F) |
| `threshold_exceeded` | `true` if temp is above the pot threshold |

---

## Configuration Reference

All tunable settings live in `include/config.h`.

| Constant | Default | Description |
|---|---|---|
| `READ_INTERVAL_MS` | 5000 | How often sensors are sampled (milliseconds) |
| `PRESSURE_HISTORY` | 10 | Number of pressure readings tracked for rain prediction |
| `RAIN_DROP_HPA` | 2.0 | Pressure drop (hPa) that triggers `rain_likely = true` |
| `POT_TEMP_MIN_F` | 50.0 | Alert threshold when pot is at minimum |
| `POT_TEMP_MAX_F` | 100.0 | Alert threshold when pot is at maximum |

---

## Libraries

| Library | Purpose |
|---|---|
| Adafruit DHT Sensor Library | DHT11 temperature & humidity |
| Adafruit BMP085 Library | BMP180 barometric pressure |
| Adafruit Unified Sensor | Shared sensor abstraction |
| ArduinoJson | JSON serialization |
| LiquidCrystal I2C | LCD display (available, not used in main sketch) |
