# ESP32 IR Remote

Biến ESP32 thành điều khiển hồng ngoại thông minh — điều khiển tivi, quạt, điều hòa cũ (không có WiFi, không có app) từ điện thoại qua trình duyệt web.

---

## Bài toán giải quyết

Các thiết bị gia dụng đời cũ (TV, quạt trần, điều hòa) chỉ có điều khiển hồng ngoại vật lý. Khi muốn điều khiển từ xa qua điện thoại hoặc tích hợp vào hệ thống smart home, không có lựa chọn nào ngoài thay mới thiết bị.

**Giải pháp:** Gắn một module ESP32 nhỏ vào phòng. ESP32 kết nối WiFi nhà, phát tín hiệu hồng ngoại thay thế điều khiển gốc, và expose web UI + REST API để điện thoại trên cùng mạng điều khiển.

```
Điện thoại  →  WiFi nhà  →  ESP32  →  IR LED  →  Thiết bị
```

Không cần cloud, không cần app cài thêm, không cần thay thiết bị.

---

## Phần cứng sử dụng

| Linh kiện | Mô tả | Vai trò |
|---|---|---|
| **ESP32 DevKit v1** | Vi điều khiển WiFi/BT | Xử lý web server + phát IR |
| **IR LED 940nm** | LED hồng ngoại (vô hình) | Phát tín hiệu điều khiển |
| **Transistor NPN 2N2222** | Khuếch đại dòng | ESP32 GPIO chỉ cấp ~12mA, LED cần 100–200mA |
| **Điện trở 100Ω** | Giới hạn dòng LED | Bảo vệ LED và transistor |
| **TSOP38238** (hoặc tương đương) | IR receiver 38kHz | Thu tín hiệu từ điều khiển gốc (chế độ học mã) |

### Sơ đồ kết nối

```
                    3V3 ──────────────────────────── VCC (TSOP38238)
                    GND ──┬───────────────────────── GND (TSOP38238)
                          │
GPIO 5 ───────────────────┴── OUT (TSOP38238)     [IR RX — học mã]

                              ┌── Collector
GPIO 4 ──[100Ω]── Base (2N2222)
                              └── Emitter ──── GND

                    3V3 ──[100Ω]── Anode (IR LED 940nm)
                                   Cathode ── Collector (2N2222)
```

> **Tại sao cần transistor?**  
> GPIO của ESP32 tối đa ~12mA. IR LED cần 100–200mA để có tầm phát đủ xa (3–5m). Transistor 2N2222 đóng vai trò switch: GPIO kích base, collector kéo dòng lớn từ 3V3 qua LED.

### Tần số sóng mang

| Giao thức | Tần số | Thiết bị phổ biến |
|---|---|---|
| NEC / Samsung / LG / Panasonic | 38 kHz | TV, quạt, DVD |
| Sony SIRC | 40 kHz | TV Sony |
| RC5 / RC6 | 36 kHz | TV Philips |

---

## Kiến trúc phần mềm

```
src/
├── config.h    — WiFi, GPIO, mã IR cho từng thiết bị
├── ir.h        — Wrapper IRsend / IRac / IRrecv
├── html.h      — Web UI nhúng (single-page app, không cần file system)
└── main.cpp    — WiFi init + WebServer routes
```

### Luồng hoạt động

```
setup()
  ├── WiFi.begin()          — kết nối WiFi nhà
  ├── irSetup()             — khởi tạo IRsend + IRrecv
  └── server.begin()        — mở HTTP server port 80

loop()
  ├── server.handleClient() — xử lý HTTP request
  └── irLearnPoll()         — polling không blocking cho chế độ học mã
```

### Điểm kỹ thuật quan trọng

**GPIO routing sau IRac:** `IRsend` và `IRac` cùng dùng GPIO 4 nhưng qua hai RMT channel khác nhau. Sau khi `IRac.sendAc()` chạy, GPIO bị route sang channel của IRac. Mỗi lần gọi `irSendNEC()` phải gọi `_irsend.begin()` trước để route lại về channel của IRsend.

**Non-blocking learn mode:** `IRrecv` polling trong `loop()` thay vì blocking wait. Web UI poll `GET /api/ir/learn` mỗi giây sau khi trigger `POST /api/ir/learn`.

---

## REST API

| Method | Endpoint | Body / Response |
|---|---|---|
| GET | `/` | Web UI (HTML) |
| GET | `/api/status` | `{ ip, rssi, ssid, uptime }` |
| POST | `/api/ir` | `{ "device":"tv"\|"fan", "cmd":"power"\|... }` |
| GET | `/api/ir/ac` | `{ power, temp, mode, fan }` |
| POST | `/api/ir/ac` | `{ "power":true, "temp":25, "mode":"cool", "fan":"auto" }` |
| POST | `/api/ir/raw` | `{ "protocol":"NEC", "code":"0xE0E040BF", "bits":32 }` |
| POST | `/api/ir/learn` | Bắt đầu học mã — `{ status:"listening" }` |
| GET | `/api/ir/learn` | `{ done:false }` hoặc `{ done:true, protocol, code, bits }` |

### Lệnh TV (`device: "tv"`)

`power` · `vol_up` · `vol_dn` · `mute` · `ch_up` · `ch_dn` · `source`

### Lệnh quạt (`device: "fan"`)

`power` · `spd1` · `spd2` · `spd3` · `swing`

### Điều hòa — mode & fan

| Trường | Giá trị hợp lệ |
|---|---|
| `mode` | `cool` · `heat` · `fan` · `dry` · `auto` |
| `fan` | `auto` · `low` · `med` · `high` |

---

## Cấu hình

Chỉnh `src/config.h` trước khi nạp:

```cpp
// 1. WiFi
#define WIFI_SSID     "ten_wifi_nha_ban"
#define WIFI_PASSWORD "mat_khau_wifi"

// 2. Hãng điều hòa
#define AC_BRAND decode_type_t::DAIKIN   // đổi thành MITSUBISHI_AC, PANASONIC_AC, ...

// 3. Mã IR (nếu không dùng chế độ học mã)
#define TV_POWER  0xE0E040BFUL           // thay bằng mã của TV nhà bạn
```

Dùng tab **Học mã** trên web UI để capture mã từ điều khiển gốc, sau đó cập nhật vào `config.h`.

---

## Build & nạp firmware

```bash
# Build
pio run -e esp32dev

# Nạp (ESP32 cắm USB vào máy, cổng COM6)
pio run -e esp32dev --target upload

# Xem log
pio device monitor --baud 115200
```

Sau khi boot, log Serial in địa chỉ IP:

```
=== ESP32 IR Remote ===
Connecting to MyWiFi....
IP: 192.168.1.105  RSSI: -52dBm
Web UI: http://192.168.1.105
```

Mở địa chỉ đó trên điện thoại (cùng mạng WiFi) để điều khiển.

---

## Thư viện

| Thư viện | Phiên bản | Mục đích |
|---|---|---|
| `crankyoldgit/IRremoteESP8266` | ^2.8.6 | Encode/decode 100+ giao thức IR |
| `bblanchon/ArduinoJson` | ^7.3.0 | Parse/serialize JSON cho REST API |
| `WebServer` (built-in Arduino ESP32) | — | HTTP server |
| `WiFi` (built-in Arduino ESP32) | — | Kết nối WiFi |
