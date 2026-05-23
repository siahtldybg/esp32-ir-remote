#pragma once

// ── WiFi ──────────────────────────────────────────────────────────────────────
#define WIFI_SSID     "TEN_WIFI_CUA_BAN"
#define WIFI_PASSWORD "MAT_KHAU_WIFI"
#define SERVER_PORT   80

// ── GPIO ──────────────────────────────────────────────────────────────────────
// IR LED phát: nối qua transistor NPN (2N2222) + điện trở 100Ω → GPIO 4
// IR receiver (học mã): TSOP38238 hoặc tương tự → GPIO 5
#define IR_TX_PIN  4
#define IR_RX_PIN  5

// ── Tivi — mặc định Samsung (38 kHz, NEC-like) ───────────────────────────────
// Thay mã phù hợp với TV của bạn. Dùng tab "Học mã" để ghi lại từ điều khiển gốc.
// LG (NEC):      POWER 0x20DF10EF  VOL+ 0x20DF40BF  VOL- 0x20DFC03F
// Sony (SIRC):   dùng sendSony() trong ir.h
#define TV_POWER   0xE0E040BFUL
#define TV_VOL_UP  0xE0E0E01FUL
#define TV_VOL_DN  0xE0E0D02FUL
#define TV_MUTE    0xE0E0F00FUL
#define TV_CH_UP   0xE0E048B7UL
#define TV_CH_DN   0xE0E008F7UL
#define TV_SOURCE  0xE0E0807FUL

// ── Quạt — mặc định NEC (Panasonic ceiling fan) ───────────────────────────────
// Thay bằng mã của quạt nhà bạn (dùng tab Học mã để ghi lại)
#define FAN_POWER  0x40040100UL
#define FAN_SPD1   0x40040300UL
#define FAN_SPD2   0x40040200UL
#define FAN_SPD3   0x40041600UL
#define FAN_SWING  0x40040800UL

// ── Điều hòa brand ────────────────────────────────────────────────────────────
// Đổi decode_type_t theo hãng điều hòa của bạn:
//   DAIKIN  MITSUBISHI_AC  PANASONIC_AC  LG  SAMSUNG_AC  GREE  TOSHIBA_AC
//   HITACHI_AC  CARRIER_AC  MIDEA  ELECTRA_AC  TCL  HAIER_AC ...
// Xem đầy đủ: github.com/crankyoldgit/IRremoteESP8266/blob/master/SupportedProtocols.md
#define AC_BRAND decode_type_t::DAIKIN
#define AC_MODEL (-1)  // -1 = model đầu tiên của hãng
