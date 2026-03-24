#include "network.h"
#include "config.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ====================================================================
//  WIFI CONNECTION
// ====================================================================
void wifi_connect() {
    Serial.printf("[INFO] Connecting to WiFi: %s\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\n[INFO] WiFi connected. IP: %s\n",
                  WiFi.localIP().toString().c_str());
}

// ====================================================================
//  BUILD JSON PAYLOAD
// ====================================================================
static String buildJson(const WeatherData &data) {
    JsonDocument doc;
    doc["temp_f"]            = serialized(String(data.tempF, 1));
    doc["temp_c"]            = serialized(String(data.tempC, 1));
    doc["humidity"]          = serialized(String(data.humidity, 1));
    doc["pressure_hpa"]      = serialized(String(data.pressureHPa, 1));
    doc["heat_index_f"]      = serialized(String(data.heatIndexF, 1));
    doc["light_pct"]         = data.lightPct;
    doc["rain_likely"]       = data.rainLikely;
    doc["alert_threshold_f"] = serialized(String(data.thresholdF, 1));
    doc["threshold_exceeded"] = data.thresholdExceeded;

    String out;
    serializeJson(doc, out);
    return out;
}

// ====================================================================
//  HTTPS POST
// ====================================================================
void http_post(const WeatherData &data) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WARN] WiFi not connected — skipping POST.");
        return;
    }

    WiFiClientSecure client;
    client.setInsecure(); // dev only — replace with client.setCACert(root_ca) in production

    HTTPClient http;
    http.begin(client, POST_URL);
    http.addHeader("Content-Type", "application/json");

    String payload = buildJson(data);
    Serial.printf("[HTTP] POST %s\n", POST_URL);
    Serial.printf("[HTTP] Payload: %s\n", payload.c_str());

    int statusCode = http.POST(payload);

    if (statusCode > 0) {
        Serial.printf("[HTTP] Response: %d\n", statusCode);
    } else {
        Serial.printf("[HTTP] POST failed: %s\n",
                      http.errorToString(statusCode).c_str());
    }

    http.end();
}
