#include "Taskbutton.h"

void TaskButtonConfig(void *pvParameters) {
  bool lastState = HIGH;
  bool triggered = false;
  unsigned long pressedAt = 0;
  const uint32_t LONG_PRESS_MS = 3000;

  for (;;) {
    int state = digitalRead(BTN_CONFIG_PIN);

    // Bắt đầu nhấn
    if (state == LOW && lastState == HIGH) {
      pressedAt = millis();
      triggered = false;
    }

    // Đủ thời gian và chưa reset trong lần nhấn này
    if (state == LOW && !triggered && millis() - pressedAt > LONG_PRESS_MS) {
      Serial.println("🔘 BOOT long-press -> clear WiFi + token/server, mở lại AP config!");
      WiFiWebManager::resetStatic();
      triggered = true;              // <-- chống lặp
    }

    lastState = state;
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
