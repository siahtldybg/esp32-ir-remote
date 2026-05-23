/*
 * ESP32 IR Remote — điều khiển TV / điều hòa / quạt qua hồng ngoại
 *
 * Kết nối WiFi nhà → truy cập http://<ESP32_IP> từ điện thoại
 *
 * REST API:
 *   GET  /api/status         — WiFi info
 *   POST /api/ir             — { "device":"tv"|"fan", "cmd":"power"|... }
 *   GET  /api/ir/ac          — trạng thái điều hòa hiện tại
 *   POST /api/ir/ac          — { "power":bool, "temp":25, "mode":"cool", "fan":"auto" }
 *   POST /api/ir/raw         — { "protocol":"NEC", "code":"0xE0E040BF", "bits":32 }
 *   POST /api/ir/learn       — bắt đầu học mã từ điều khiển gốc
 *   GET  /api/ir/learn       — kết quả học mã
 */

#include <WiFi.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <ArduinoJson.h>
#include "config.h"
#include "wifi_cfg.h"
#include "ir.h"
#include "devices.h"
#include "html.h"

WebServer server(SERVER_PORT);

// ── CORS ──────────────────────────────────────────────────────────────────────

void addCors() {
  server.sendHeader("Access-Control-Allow-Origin",  "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// ── Route handlers ────────────────────────────────────────────────────────────

void handleRoot()    { addCors(); server.send_P(200, "text/html", INDEX_HTML); }
void handleOptions() { addCors(); server.send(204); }

void handleStatus() {
  addCors();
  JsonDocument doc;
  doc["ip"]     = WiFi.localIP().toString();
  doc["rssi"]   = WiFi.RSSI();
  doc["ssid"]   = WiFi.SSID();
  doc["uptime"] = millis() / 1000;
  String body; serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void handleIr() {
  addCors();
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"missing body\"}"); return;
  }
  JsonDocument req;
  if (deserializeJson(req, server.arg("plain"))) {
    server.send(400, "application/json", "{\"error\":\"invalid json\"}"); return;
  }
  String device = req["device"] | "";
  String cmd    = req["cmd"]    | "";
  bool ok = false;
  if (device == "tv")  ok = irTvCmd(cmd);
  if (device == "fan") ok = irFanCmd(cmd);
  server.send(ok ? 200 : 400, "application/json",
    ok ? "{\"ok\":true}" : "{\"error\":\"unknown device or command\"}");
}

void handleIrAcGet() {
  addCors();
  const AcState& s = irAcState();
  JsonDocument doc;
  doc["power"] = s.power;
  doc["temp"]  = s.temp;
  doc["mode"]  = s.mode;
  doc["fan"]   = s.fan;
  String body; serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void handleIrAcPost() {
  addCors();
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"missing body\"}"); return;
  }
  JsonDocument req;
  if (deserializeJson(req, server.arg("plain"))) {
    server.send(400, "application/json", "{\"error\":\"invalid json\"}"); return;
  }
  const AcState& cur = irAcState();
  bool   power = req["power"].is<bool>()   ? req["power"].as<bool>()   : cur.power;
  float  temp  = req["temp"].is<float>()   ? req["temp"].as<float>()   : cur.temp;
  String mode  = req["mode"].is<const char*>() ? req["mode"].as<String>()  : cur.mode;
  String fan   = req["fan"].is<const char*>()  ? req["fan"].as<String>()   : cur.fan;
  if (!irAcSend(power, temp, mode, fan)) {
    server.send(502, "application/json", "{\"error\":\"ir send failed\"}"); return;
  }
  handleIrAcGet();
}

void handleIrRaw() {
  addCors();
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"missing body\"}"); return;
  }
  JsonDocument req;
  if (deserializeJson(req, server.arg("plain"))) {
    server.send(400, "application/json", "{\"error\":\"invalid json\"}"); return;
  }
  String   proto   = req["protocol"] | "NEC";
  String   codeStr = req["code"]     | "0";
  uint64_t code    = strtoull(codeStr.c_str(), nullptr, 16);
  uint16_t bits    = req["bits"] | 32;
  bool ok = irRawSend(proto, code, bits);
  server.send(ok ? 200 : 400, "application/json",
    ok ? "{\"ok\":true}" : "{\"error\":\"unsupported protocol\"}");
}

void handleIrLearnPost() {
  addCors();
  irStartLearn();
  server.send(200, "application/json", "{\"status\":\"listening\"}");
}

// ── Device Registry routes ────────────────────────────────────────────────────

void handleDevicesGet() {
  addCors();
  server.send(200, "application/json", devicesGetAll());
}

void handleDevicesPost() {
  addCors();
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"missing body\"}"); return;
  }
  String err = devicesCreate(server.arg("plain"));
  if (err.isEmpty()) server.send(200, "application/json", "{\"ok\":true}");
  else server.send(400, "application/json", "{\"error\":\"" + err + "\"}");
}

void handleDevicesDelete() {
  addCors();
  String id = server.pathArg(0);
  if (devicesDelete(id)) server.send(200, "application/json", "{\"ok\":true}");
  else server.send(404, "application/json", "{\"error\":\"not found\"}");
}

void handleDevicesAddCmd() {
  addCors();
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"missing body\"}"); return;
  }
  String id  = server.pathArg(0);
  String err = devicesAddCmd(id, server.arg("plain"));
  if (err.isEmpty()) server.send(200, "application/json", "{\"ok\":true}");
  else server.send(400, "application/json", "{\"error\":\"" + err + "\"}");
}

void handleDevicesSendCmd() {
  addCors();
  String id  = server.pathArg(0);
  String cmd = server.pathArg(1);
  String proto; uint64_t code = 0; uint16_t bits = 32;
  if (!devicesGetCmd(id, cmd, proto, code, bits)) {
    server.send(404, "application/json", "{\"error\":\"device or command not found\"}"); return;
  }
  bool ok = irRawSend(proto, code, bits);
  server.send(ok ? 200 : 400, "application/json",
    ok ? "{\"ok\":true}" : "{\"error\":\"unsupported protocol\"}");
}

// ── WiFi config routes ────────────────────────────────────────────────────────

static bool _pendingRestart = false;

void handleWifiGet() {
  addCors();
  JsonDocument doc;
  doc["ssid"] = WiFi.SSID();
  doc["ip"]   = WiFi.localIP().toString();
  String body; serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void handleWifiPost() {
  addCors();
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"missing body\"}"); return;
  }
  JsonDocument req;
  if (deserializeJson(req, server.arg("plain"))) {
    server.send(400, "application/json", "{\"error\":\"invalid json\"}"); return;
  }
  String ssid = req["ssid"]     | "";
  String pass = req["password"] | "";
  if (ssid.isEmpty()) {
    server.send(400, "application/json", "{\"error\":\"ssid required\"}"); return;
  }
  if (!wifiCfgSave(ssid, pass)) {
    server.send(500, "application/json", "{\"error\":\"save failed\"}"); return;
  }
  server.send(200, "application/json", "{\"ok\":true,\"restarting\":true}");
  _pendingRestart = true;
}

void handleIrLearnGet() {
  addCors();
  String proto, codeHex; uint16_t bits = 0;
  bool done = irLearnResult(proto, codeHex, bits);
  JsonDocument doc;
  doc["done"] = done;
  if (done) {
    doc["protocol"] = proto;
    doc["code"]     = codeHex;
    doc["bits"]     = bits;
  }
  String body; serializeJson(doc, body);
  server.send(200, "application/json", body);
}

// ── Setup ─────────────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 IR Remote ===");

  String wSsid = WIFI_SSID, wPass = WIFI_PASSWORD;
  if (wifiCfgLoad(wSsid, wPass))
    Serial.println("[WiFi] Using saved credentials");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wSsid.c_str(), wPass.c_str());
  Serial.printf("Connecting to %s", wSsid.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500); Serial.print("."); attempts++;
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi FAILED — restarting in 3s");
    delay(3000); ESP.restart();
  }
  Serial.printf("\nIP: %s  RSSI: %ddBm\n",
    WiFi.localIP().toString().c_str(), WiFi.RSSI());

  irSetup();

  server.on("/",               HTTP_GET,     handleRoot);
  server.on("/api/status",     HTTP_GET,     handleStatus);
  server.on("/api/ir",         HTTP_POST,    handleIr);
  server.on("/api/ir",         HTTP_OPTIONS, handleOptions);
  server.on("/api/ir/ac",      HTTP_GET,     handleIrAcGet);
  server.on("/api/ir/ac",      HTTP_POST,    handleIrAcPost);
  server.on("/api/ir/ac",      HTTP_OPTIONS, handleOptions);
  server.on("/api/ir/raw",     HTTP_POST,    handleIrRaw);
  server.on("/api/ir/raw",     HTTP_OPTIONS, handleOptions);
  server.on("/api/ir/learn",   HTTP_POST,    handleIrLearnPost);
  server.on("/api/ir/learn",   HTTP_GET,     handleIrLearnGet);

  server.on("/api/devices",              HTTP_GET,     handleDevicesGet);
  server.on("/api/devices",              HTTP_POST,    handleDevicesPost);
  server.on("/api/devices",              HTTP_OPTIONS, handleOptions);
  server.on(UriBraces("/api/devices/{}"),          HTTP_DELETE,  handleDevicesDelete);
  server.on(UriBraces("/api/devices/{}/cmds"),     HTTP_POST,    handleDevicesAddCmd);
  server.on(UriBraces("/api/devices/{}/cmds"),     HTTP_OPTIONS, handleOptions);
  server.on(UriBraces("/api/devices/{}/cmd/{}"),   HTTP_POST,    handleDevicesSendCmd);

  server.on("/api/wifi",   HTTP_GET,     handleWifiGet);
  server.on("/api/wifi",   HTTP_POST,    handleWifiPost);
  server.on("/api/wifi",   HTTP_OPTIONS, handleOptions);

  server.begin();
  Serial.printf("Web UI: http://%s\n", WiFi.localIP().toString().c_str());
}

// ── Loop ──────────────────────────────────────────────────────────────────────

void loop() {
  server.handleClient();
  irLearnPoll();
  if (_pendingRestart) { delay(500); ESP.restart(); }
}
