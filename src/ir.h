#pragma once
/*
 * IR send/receive wrapper
 *
 * TX: IRsend (NEC) cho TV + quạt; IRac (multi-brand) cho điều hòa.
 *     Cả hai dùng chung IR_TX_PIN. irSendNEC() gọi begin() trước mỗi lần gửi
 *     để đảm bảo GPIO routing đúng sau khi IRac đã dùng pin.
 *
 * RX: IRrecv trên IR_RX_PIN — non-blocking, polling trong loop().
 *     Dùng cho chế độ học mã (tap "Học mã" trên web UI).
 *
 * Tần số sóng mang:
 *   - NEC / Samsung / LG / Panasonic: 38 kHz (mặc định)
 *   - Sony SIRC: 40 kHz
 *   - RC5 / RC6: 36 kHz
 *   - /api/ir/raw cho phép chỉ định giao thức tùy ý (Mi remote style)
 */

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRac.h>
#include <IRutils.h>
#include "config.h"

static IRsend _irsend(IR_TX_PIN);
static IRac   _irac(IR_TX_PIN);
static IRrecv _irrecv(IR_RX_PIN, /*bufsize=*/1024, /*timeout=*/15, /*save_buf=*/true);

// ── AC state ──────────────────────────────────────────────────────────────────

struct AcState {
  bool   power = false;
  float  temp  = 25.0f;
  String mode  = "cool";  // cool heat fan dry auto
  String fan   = "auto";  // auto low med high
};
static AcState _acState;

// ── Learn state ───────────────────────────────────────────────────────────────

static bool          _learning  = false;
static bool          _learnDone = false;
static decode_results _learnRes;

// ── Helpers ───────────────────────────────────────────────────────────────────

static stdAc::opmode_t _modeOf(const String& m) {
  if (m == "heat") return stdAc::opmode_t::kHeat;
  if (m == "fan")  return stdAc::opmode_t::kFan;
  if (m == "dry")  return stdAc::opmode_t::kDry;
  if (m == "auto") return stdAc::opmode_t::kAuto;
  return stdAc::opmode_t::kCool;
}

static stdAc::fanspeed_t _fanOf(const String& f) {
  if (f == "low")  return stdAc::fanspeed_t::kLow;
  if (f == "med")  return stdAc::fanspeed_t::kMedium;
  if (f == "high") return stdAc::fanspeed_t::kHigh;
  return stdAc::fanspeed_t::kAuto;
}

// ── Public: setup ─────────────────────────────────────────────────────────────

void irSetup() {
  _irsend.begin();
  _irrecv.enableIRIn();
  Serial.printf("[IR] TX=GPIO%d  RX=GPIO%d\n", IR_TX_PIN, IR_RX_PIN);
}

// ── Public: TV / Fan (NEC) ────────────────────────────────────────────────────

static bool irSendNEC(uint32_t code) {
  _irsend.begin();  // re-route GPIO → TX channel (may have changed after IRac send)
  _irsend.sendNEC(code);
  Serial.printf("[IR] NEC 0x%08X\n", code);
  return true;
}

bool irTvCmd(const String& cmd) {
  uint32_t code = 0;
  if      (cmd == "power")  code = TV_POWER;
  else if (cmd == "vol_up") code = TV_VOL_UP;
  else if (cmd == "vol_dn") code = TV_VOL_DN;
  else if (cmd == "mute")   code = TV_MUTE;
  else if (cmd == "ch_up")  code = TV_CH_UP;
  else if (cmd == "ch_dn")  code = TV_CH_DN;
  else if (cmd == "source") code = TV_SOURCE;
  else return false;
  return irSendNEC(code);
}

bool irFanCmd(const String& cmd) {
  uint32_t code = 0;
  if      (cmd == "power") code = FAN_POWER;
  else if (cmd == "spd1")  code = FAN_SPD1;
  else if (cmd == "spd2")  code = FAN_SPD2;
  else if (cmd == "spd3")  code = FAN_SPD3;
  else if (cmd == "swing") code = FAN_SWING;
  else return false;
  return irSendNEC(code);
}

// ── Public: raw send (Mi remote style — giao thức + tần số tùy chọn) ──────────

bool irRawSend(const String& proto, uint64_t code, uint16_t bits) {
  _irsend.begin();
  if (proto == "NEC")     { _irsend.sendNEC(code, bits);     }
  else if (proto == "SAMSUNG") { _irsend.sendSAMSUNG(code, bits); }
  else if (proto == "LG")      { _irsend.sendLG(code, bits);      }
  else if (proto == "SONY")    { _irsend.sendSony(code, bits);     }
  else if (proto == "RC5")     { _irsend.sendRC5(code, bits);      }
  else if (proto == "RC6")     { _irsend.sendRC6(code, bits);      }
  else if (proto == "PANASONIC") { _irsend.sendPanasonic64(code, bits); }
  else return false;
  Serial.printf("[IR] %s %llu bits=%d\n", proto.c_str(), code, bits);
  return true;
}

// ── Public: AC (multi-brand via IRac) ────────────────────────────────────────

bool irAcSend(bool power, float temp, const String& mode, const String& fan) {
  _acState.power = power;
  _acState.temp  = temp;
  _acState.mode  = mode;
  _acState.fan   = fan;
  _irac.next.protocol = AC_BRAND;
  _irac.next.model    = AC_MODEL;
  _irac.next.power    = power;
  _irac.next.mode     = _modeOf(mode);
  _irac.next.degrees  = temp;
  _irac.next.celsius  = true;
  _irac.next.fanspeed = _fanOf(fan);
  _irac.next.swingv   = stdAc::swingv_t::kAuto;
  _irac.next.swingh   = stdAc::swingh_t::kOff;
  _irac.next.quiet    = false;
  _irac.next.turbo    = false;
  _irac.next.light    = false;
  _irac.next.sleep    = -1;
  _irac.next.clock    = -1;
  bool ok = _irac.sendAc();
  Serial.printf("[IR] AC power=%d temp=%.0f mode=%s fan=%s %s\n",
    power, temp, mode.c_str(), fan.c_str(), ok ? "OK" : "FAIL");
  return ok;
}

const AcState& irAcState() { return _acState; }

// ── Public: learn (non-blocking) ─────────────────────────────────────────────

void irStartLearn() {
  _learnDone = false;
  _learning  = true;
  _irrecv.resume();
  Serial.println("[IR] Learning started");
}

// Call every loop() — returns true when a code is captured
bool irLearnPoll() {
  if (!_learning) return false;
  if (_irrecv.decode(&_learnRes)) {
    _learning  = false;
    _learnDone = true;
    _irrecv.resume();
    Serial.printf("[IR] Learned: %s 0x%llX %dbits\n",
      typeToString(_learnRes.decode_type).c_str(),
      _learnRes.value, _learnRes.bits);
    return true;
  }
  return false;
}

// Returns false if not yet done
bool irLearnResult(String& proto, String& codeHex, uint16_t& bits) {
  if (!_learnDone) return false;
  proto   = typeToString(_learnRes.decode_type);
  bits    = _learnRes.bits;
  char buf[20];
  snprintf(buf, sizeof(buf), "0x%llX", _learnRes.value);
  codeHex = buf;
  return true;
}
