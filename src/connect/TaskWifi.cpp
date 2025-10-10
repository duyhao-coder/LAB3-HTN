#include "TaskWifi.h"


void InitWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("✅ Connected to WiFi!");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  xTaskCreate(TaskWiFi, "TaskWiFi", 4096, NULL, 1, NULL); 
}


bool reconnectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  Serial.println("⚠️  WiFi disconnected. Reconnecting...");
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Reconnected to WiFi!");
    return true;
  } else {
    Serial.println("\n❌ Failed to reconnect WiFi.");
    return false;
  }
}

// Task quản lý WiFi định kỳ
void TaskWiFi(void *pvParameters) {
  while (true) {
    if (WiFi.status() != WL_CONNECTED) {
      reconnectWiFi();
    }
    vTaskDelay(WIFI_RECONNECT_DELAY / portTICK_PERIOD_MS);
  }
}
