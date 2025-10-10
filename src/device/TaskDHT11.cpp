#include "TaskDHT11.h"

// Tạo đối tượng cảm biến DHT
DHT dht(DHTPIN, DHTTYPE);

// 🔹 Hàm khởi tạo cảm biến và tạo task
void InitDHT11() {
  dht.begin();
  Serial.println("DHT11 initialized successfully.");

  // Tạo 2 task song song để đọc nhiệt độ và độ ẩm
  xTaskCreate(TaskTemperature, "TaskTemperature", 2048, NULL, 1, NULL);
  xTaskCreate(TaskHumidity, "TaskHumidity", 2048, NULL, 1, NULL);
}

// 🔹 Task đọc nhiệt độ
void TaskTemperature(void *pvParameters) {
  while (true) {
    float temp = getTemperature();
    if (temp != -1) {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.println(" °C");
    } else {
      Serial.println("Failed to read temperature from DHT11!");
    }
    vTaskDelay(TEMP_DELAY / portTICK_PERIOD_MS);
  }
}

// 🔹 Task đọc độ ẩm
void TaskHumidity(void *pvParameters) {
  while (true) {
    float hum = getHumidity();
    if (hum != -1) {
      Serial.print("Humidity: ");
      Serial.print(hum);
      Serial.println(" %");
    } else {
      Serial.println("Failed to read humidity from DHT11!");
    }
    vTaskDelay(HUMI_DELAY / portTICK_PERIOD_MS);
  }
}

// 🔹 Hàm đọc nhiệt độ
float getTemperature() {
  float t = dht.readTemperature(); // Đọc nhiệt độ °C
  if (isnan(t)) return -1;
  return t;
}

// 🔹 Hàm đọc độ ẩm
float getHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) return -1;
  return h;
}
