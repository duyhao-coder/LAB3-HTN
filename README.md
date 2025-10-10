
<a href="https://github.com/ACLAB-HCMUT"><img src="https://raw.githubusercontent.com/ACLAB-HCMUT/Common/main/Assets/ACLAB_IMG_1.png" alt="ACLAB logo" title="ACLAB" align="right" height="100" /></a>

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/luos/library/luos_engine.svg)](https://registry.platformio.org/libraries/luos/luos_engine)

# 🧠 Lab 1 – Hệ Thống Nhúng

## 🎯 Giới thiệu
Dự án **Lab 1** của môn **Hệ Thống Nhúng**, được phát triển trên nền tảng **ESP32** sử dụng các công nghệ:
- Kết nối **WiFi** và **MQTT** với **ThingsBoard / CoreIoT**
- Giao tiếp cảm biến **DHT20** đo nhiệt độ và độ ẩm
- Điều khiển **LED / NeoPixel** bằng **RPC** và **shared attributes**
- Tổ chức chương trình theo **FreeRTOS task**

Mục tiêu:  
Xây dựng hệ thống IoT nhúng cơ bản, có khả năng đọc dữ liệu cảm biến, gửi lên nền tảng đám mây và nhận điều khiển từ xa.

---


---

## 🧩 Thành viên nhóm

| MSSV     | Họ và Tên              |
|-----------|------------------------|
| 2213416   | VÕ HUỲNH ANH THƯ      |
| 2212725   | DANH NHỰT QUANG        |
| 2210505   | LÊ NGUYỄN ANH DUY     |
| 2210512   | NGUYỄN HẠO DUY        |

---

## 💡 Mô tả chức năng chính
- **WiFi Task**: Tự động kết nối lại WiFi nếu mất kết nối.  
- **CoreIoT Task**: Duy trì kết nối MQTT với ThingsBoard, quản lý RPC và thuộc tính chia sẻ.  
- **Telemetry Task**: Gửi dữ liệu cảm biến (nhiệt độ, độ ẩm) định kỳ.  
- **Attribute Task**: Gửi các thông tin WiFi (RSSI, SSID, IP).  
- **NeoPixel Task**: Đổi màu LED theo tín hiệu từ RPC (nút điều khiển trên dashboard).  

---
<img width="1612" height="854" alt="image" src="https://github.com/user-attachments/assets/bb2b6fb7-aec7-437d-b12c-3117c1955be4" />

## 🧠 Công nghệ & thư viện sử dụng
- **Arduino Framework**
- **ESP32 (FreeRTOS)**
- **WiFi.h**, **Arduino_MQTT_Client**, **ThingsBoard.h**
- **DHT20 Sensor**
- **Adafruit NeoPixel**
- **ESPAsyncWebServer**, **ArduinoJson**
- **ElegantOTA** (nếu dùng OTA cập nhật firmware)

---

## 🚀 Cách sử dụng
1. Mở project bằng **PlatformIO** hoặc **Arduino IDE**.  
2. Cấu hình WiFi và Token trong `GlobalConfig.h`.  
3. Nạp code vào ESP32.  
4. Mở Serial Monitor để xem log.  
5. Truy cập **ThingsBoard/CoreIoT dashboard** để điều khiển và quan sát dữ liệu.

---

## 📜 Giấy phép
Dự án được thực hiện phục vụ mục đích học tập trong môn **Hệ Thống Nhúng** – Không sử dụng cho mục đích thương mại.

