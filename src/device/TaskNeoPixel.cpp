#include "TaskNeoPixel.h"

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

volatile int colorIndex = 0;
volatile bool buttonPressed = false;

QueueHandle_t ledCommandQueue = nullptr;

uint32_t colors[] = {
  strip.Color(255, 0, 0),     // Đỏ
  strip.Color(0, 255, 0),     // Xanh lá
  strip.Color(0, 0, 255),     // Xanh dương
  strip.Color(255, 0, 255),   // Tím
  strip.Color(255, 255, 255), // Trắng
  strip.Color(0, 0, 0)        // Tắt
};

// Hàm khởi tạo LED
void InitNeoPixel() {
  strip.begin();
  strip.show();
  Serial.println("NeoPixel initialized successfully.");

  // Tạo hàng đợi nhận lệnh từ CoreIoT
  ledCommandQueue = xQueueCreate(5, sizeof(bool));
  if (ledCommandQueue == nullptr) {
    Serial.println("[NeoPixel] ❌ Failed to create command queue!");
  }

  // Tạo task LED
  xTaskCreate(TaskNeoPixel, "TaskNeoPixel", 2048, NULL, 1, NULL);
}

// Task LED đổi màu dựa trên trạng thái buttonPressed
void TaskNeoPixel(void *pvParameters) {
  bool command = false;

  while (true) {
    // Nếu có lệnh mới từ CoreIoT → cập nhật trạng thái
    if (xQueueReceive(ledCommandQueue, &command, 0)) {
      buttonPressed = command;
      Serial.printf("[NeoPixel] Received new state: %s\n", buttonPressed ? "ON" : "OFF");

      if (!buttonPressed) {
        strip.clear();
        strip.show();
      }
    }

    // Khi bật → đổi màu liên tục
    if (buttonPressed) {
      colorIndex = (colorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
      strip.setPixelColor(0, colors[colorIndex]);
      strip.show();
      Serial.printf("[NeoPixel] Auto changed color index: %d\n", colorIndex);
    }

    vTaskDelay(100  / portTICK_PERIOD_MS);
  }
}
