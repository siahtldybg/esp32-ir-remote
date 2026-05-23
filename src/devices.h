#pragma once
/*
 * Device Registry — lưu danh sách thiết bị + mã IR vào LittleFS (devices.json)
 *
 * Schema:
 *   [ { "id":"...", "name":"...",
 *       "cmds": { "power":{"proto":"NEC","code":"0x...","bits":32}, ... } },
 *     ... ]
 *
 * Không cần reflash khi thêm thiết bị mới.
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#define DEVICES_FILE "/devices.json"
#define DEVICES_MAX_SIZE 8192

// ── Internal helpers ──────────────────────────────────────────────────────────

static bool _fsReady = false;

static bool _ensureFs() {
  if (_fsReady) return true;
  _fsReady = LittleFS.begin(true);  // true = format if mount fails
  if (!_fsReady) Serial.println("[FS] LittleFS mount failed");
  return _fsReady;
}

static String _readFile() {
  if (!_ensureFs()) return "[]";
  if (!LittleFS.exists(DEVICES_FILE)) return "[]";
  File f = LittleFS.open(DEVICES_FILE, "r");
  if (!f) return "[]";
  String s = f.readString();
  f.close();
  return s;
}

static bool _writeFile(const String& json) {
  if (!_ensureFs()) return false;
  File f = LittleFS.open(DEVICES_FILE, "w");
  if (!f) return false;
  f.print(json);
  f.close();
  return true;
}

// ── Public API ────────────────────────────────────────────────────────────────

// GET /api/devices — trả về toàn bộ mảng JSON
String devicesGetAll() {
  return _readFile();
}

// POST /api/devices — tạo thiết bị mới: { "id":"...", "name":"..." }
// Trả về "" nếu thành công, hoặc chuỗi lỗi
String devicesCreate(const String& body) {
  JsonDocument req;
  if (deserializeJson(req, body)) return "invalid json";
  String id   = req["id"]   | "";
  String name = req["name"] | "";
  if (id.isEmpty() || name.isEmpty()) return "id and name required";

  JsonDocument arr;
  deserializeJson(arr, _readFile());
  for (JsonObject dev : arr.as<JsonArray>())
    if (String(dev["id"] | "") == id) return "id already exists";

  JsonObject newDev = arr.as<JsonArray>().add<JsonObject>();
  newDev["id"]   = id;
  newDev["name"] = name;
  newDev["cmds"] = JsonObject();  // empty

  String out;
  serializeJson(arr, out);
  return _writeFile(out) ? "" : "write failed";
}

// DELETE /api/devices/:id
bool devicesDelete(const String& id) {
  JsonDocument arr;
  deserializeJson(arr, _readFile());
  JsonArray a = arr.as<JsonArray>();
  for (size_t i = 0; i < a.size(); i++) {
    if (String(a[i]["id"] | "") == id) {
      a.remove(i);
      String out; serializeJson(arr, out);
      return _writeFile(out);
    }
  }
  return false;
}

// POST /api/devices/:id/cmds — thêm/ghi đè lệnh:
//   { "cmd":"power", "proto":"NEC", "code":"0xE0E040BF", "bits":32 }
String devicesAddCmd(const String& id, const String& body) {
  JsonDocument req;
  if (deserializeJson(req, body)) return "invalid json";
  String cmd   = req["cmd"]   | "";
  String proto = req["proto"] | "";
  String code  = req["code"]  | "";
  int    bits  = req["bits"]  | 32;
  String label = req["label"] | "";
  if (label.isEmpty()) label = cmd;
  if (cmd.isEmpty() || proto.isEmpty() || code.isEmpty()) return "cmd, proto, code required";

  JsonDocument arr;
  deserializeJson(arr, _readFile());
  for (JsonObject dev : arr.as<JsonArray>()) {
    if (String(dev["id"] | "") == id) {
      JsonObject cmds = dev["cmds"].as<JsonObject>();
      cmds[cmd]["label"] = label;
      cmds[cmd]["proto"] = proto;
      cmds[cmd]["code"]  = code;
      cmds[cmd]["bits"]  = bits;
      String out; serializeJson(arr, out);
      return _writeFile(out) ? "" : "write failed";
    }
  }
  return "device not found";
}

// POST /api/devices/:id/cmd/:cmd — lấy thông tin IR của lệnh để gửi
// Trả về false nếu không tìm thấy; điền proto/code/bits nếu tìm thấy
bool devicesGetCmd(const String& id, const String& cmd,
                   String& proto, uint64_t& code, uint16_t& bits) {
  JsonDocument arr;
  deserializeJson(arr, _readFile());
  for (JsonObject dev : arr.as<JsonArray>()) {
    if (String(dev["id"] | "") == id) {
      JsonObject c = dev["cmds"][cmd];
      if (c.isNull()) return false;
      proto = String(c["proto"] | "NEC");
      bits  = c["bits"] | 32;
      String codeStr = c["code"] | "0";
      code  = strtoull(codeStr.c_str(), nullptr, 16);
      return true;
    }
  }
  return false;
}
