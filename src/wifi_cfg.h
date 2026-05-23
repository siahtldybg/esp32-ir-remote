#pragma once
/*
 * WiFi config lưu trên LittleFS (/wifi.json).
 * Ưu tiên hơn WIFI_SSID/WIFI_PASSWORD trong config.h.
 * Được ghi bởi POST /api/wifi — ESP32 tự restart sau đó.
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#define WIFI_CFG_FILE "/wifi.json"

// Đọc credentials đã lưu; trả false nếu chưa có hoặc mount lỗi
bool wifiCfgLoad(String& ssid, String& password) {
  if (!LittleFS.begin(true)) return false;
  if (!LittleFS.exists(WIFI_CFG_FILE)) return false;
  File f = LittleFS.open(WIFI_CFG_FILE, "r");
  if (!f) return false;
  JsonDocument doc;
  bool ok = !deserializeJson(doc, f);
  f.close();
  if (!ok) return false;
  String s = doc["ssid"] | "";
  if (s.isEmpty()) return false;
  ssid     = s;
  password = doc["password"] | "";
  return true;
}

bool wifiCfgSave(const String& ssid, const String& password) {
  if (!LittleFS.begin(true)) return false;
  File f = LittleFS.open(WIFI_CFG_FILE, "w");
  if (!f) return false;
  JsonDocument doc;
  doc["ssid"]     = ssid;
  doc["password"] = password;
  serializeJson(doc, f);
  f.close();
  return true;
}
