// #include "globals.h"

// void setup() {
//   Serial.begin(115200);
//   delay( 1000);
//   // InitDHT11();
//   // InitNeoPixel();
//   InitWiFi();
// }

// void loop() {

// }

#include "globals.h"

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);


volatile bool attributesChanged = false;
volatile bool ledState = false;
volatile uint16_t blinkingInterval = 1000U;

void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);
  InitNeoPixel();
  InitWiFi();

  // --- CREATE TASKS ---
  xTaskCreatePinnedToCore(TaskCoreIoT, "TaskCoreIoT", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskTelemetry, "TaskTelemetry", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskAttributes, "TaskAttributes", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskTbLoop, "TaskTbLoop", 4096, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
