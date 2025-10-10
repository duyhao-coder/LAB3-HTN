#ifndef TASK_WIFI_H
#define TASK_WIFI_H

#include "globals.h"



// Thời gian chờ kết nối lại
#define WIFI_RECONNECT_DELAY 5000

// 🧩 Khai báo hàm
void InitWiFi();          // Khởi tạo WiFi ban đầu
bool reconnectWiFi();     // Kiểm tra và tự động kết nối lại
void TaskWiFi(void *pvParameters); // Task quản lý WiFi

#endif
