#include "TaskCoreIoT.h"

void TaskCoreIoT(void *pvParameters) {
  for (;;) {
    if (!connectCoreIoT()) {
      Serial.println("[TaskCoreIoT] Reconnecting CoreIoT...");
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void TaskTelemetry(void *pvParameters) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED && tb.connected()) {
      sendTelemetryData();
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

void TaskAttributes(void *pvParameters) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED && tb.connected()) {
      sendAttributesData();
    }
    vTaskDelay(30000 / portTICK_PERIOD_MS);
  }
}

void TaskTbLoop(void *pvParameters) {
  for (;;) {
    if (tb.connected()) tb.loop();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
