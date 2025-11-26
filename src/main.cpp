//test demo lab1

#include "globals.h"


WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);
WebDashboard dashboard;

void TaskWiFiManager(void *pvParameters) {
  for (;;) {
    WiFiWebManager::loopTask();   // gọi loop() của WiFiWebManager
    vTaskDelay(pdMS_TO_TICKS(100));  // 100ms/lần là đủ
  }
}
float generateRandomTemperature() {
  return random(150, 300) / 10.0;  // Nhiệt độ ngẫu nhiên từ 15°C đến 30°C
}

float generateRandomHumidity() {
  return random(400, 700) / 10.0;  // Độ ẩm ngẫu nhiên từ 40% đến 70%
}

// Hàm task phục vụ WebServer
void TaskWebServer(void *pvParameters) {
  static unsigned long lastUpdate = 0;
  while (true) {
    dashboard.handleClient();  // Xử lý các request từ client

    // Cập nhật cảm biến mỗi 5 giây
    if (millis() - lastUpdate > 5000) {
      float temp = generateRandomTemperature();  // Sinh nhiệt độ ngẫu nhiên
      float hum = generateRandomHumidity();     // Sinh độ ẩm ngẫu nhiên
      dashboard.updateSensorData(temp, hum);    // Cập nhật dữ liệu cảm biến
      lastUpdate = millis();  // Cập nhật thời gian
    }


    vTaskDelay(10 / portTICK_PERIOD_MS);  // Thời gian trễ để tránh chiếm dụng quá nhiều CPU
  }
}



void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);
  // InitWiFi();
  // bool wifiOk = WiFiWebManager::start("ESP32-Setup", "12345678", 10000);

  // --- CREATE TASKS ---
  // xTaskCreatePinnedToCore(TaskCoreIoT, "TaskCoreIoT", 4096, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(TaskTelemetry, "TaskTelemetry", 4096, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(TaskAttributes, "TaskAttributes", 4096, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(TaskTbLoop, "TaskTbLoop", 4096, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(led_blinky, "led_blinky", 2048, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(neo_blinky, "neo_blinky", 2048, NULL, 1, NULL, 1);  // Chạy trên core 1
//   xTaskCreatePinnedToCore(TaskWiFiManager,"TaskWiFiManager",4096,NULL,2,NULL,1);
//   xTaskCreatePinnedToCore(TaskButtonConfig,"TaskButtonConfig",2048,NULL,1,NULL,1);
// if (wifiOk) {
//   Serial.println("WiFi Connected!");
//   Serial.print("IP Address: ");
//   Serial.println(WiFi.localIP());  // In ra địa chỉ IP của ESP32
//   xTaskCreatePinnedToCore(TaskWebServer, "TaskWebServer", 4096, NULL, 2, NULL, 1);
// }
     xTaskCreate( tiny_ml_task, "Tiny ML Task" ,2048  ,NULL  ,2 , NULL); 

}


void loop() {
  vTaskDelay(portMAX_DELAY);
}
