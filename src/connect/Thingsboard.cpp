#include "Thingsboard.h"

constexpr std::array<const char *, 2U> SHARED_ATTRIBUTES_LIST = {
  "ledState", "blinkingInterval"
};

const std::array<RPC_Callback, 1U> callbacks = {
  RPC_Callback{"setLedSwitchValue", setLedSwitchState}
};

const Shared_Attribute_Callback attributes_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());
const Attribute_Request_Callback attribute_shared_request_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());

bool connectCoreIoT() {
  if (tb.connected()) return true;

  Serial.print("[CoreIoT] Connecting to ");
  Serial.println(THINGSBOARD_SERVER);
  if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
    Serial.println("[CoreIoT] ❌ Connect failed!");
    return false;
  }

  tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());

  if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend())) {
    Serial.println("[CoreIoT] RPC subscribe failed");
    return false;
  }

  if (!tb.Shared_Attributes_Subscribe(attributes_callback)) {
    Serial.println("[CoreIoT] Attribute subscribe failed");
    return false;
  }

  if (!tb.Shared_Attributes_Request(attribute_shared_request_callback)) {
    Serial.println("[CoreIoT] Attribute request failed");
    return false;
  }

  Serial.println("[CoreIoT] ✅ Connected!");
  return true;
}

void sendTelemetryData() {
  float temperature = random(200, 320) / 10.0;
  float humidity = random(400, 900) / 10.0;

  Serial.printf("[Telemetry] Temp: %.2f °C | Hum: %.2f %%\n", temperature, humidity);
  tb.sendTelemetryData("temperature", temperature);
  tb.sendTelemetryData("humidity", humidity);
}

void sendAttributesData() {
  tb.sendAttributeData("rssi", WiFi.RSSI());
  tb.sendAttributeData("ssid", WiFi.SSID().c_str());
  tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());
}

RPC_Response setLedSwitchState(const RPC_Data &data) {
  bool newState = data;
  Serial.printf("[RPC] LED button pressed -> %d\n", newState);

  // Gửi lệnh vào Queue cho TaskNeoPixel
  if (ledCommandQueue != nullptr) {
    xQueueSend(ledCommandQueue, &newState, 0);
  } else {
    Serial.println("[RPC] ⚠️ Queue not ready!");
  }

  ledState = newState;
  attributesChanged = true;
  return RPC_Response("setLedSwitchValue", newState);
}
void processSharedAttributes(const Shared_Attribute_Data &data) {
  for (auto it = data.begin(); it != data.end(); ++it) {
    if (strcmp(it->key().c_str(), "blinkingInterval") == 0) {
      blinkingInterval = it->value().as<uint16_t>();
      Serial.printf("[Attr] Blink interval -> %u\n", blinkingInterval);
    } else if (strcmp(it->key().c_str(), "ledState") == 0) {
      bool newState = it->value().as<bool>();
      Serial.printf("[Attr] LED state -> %d\n", newState);

      // Gửi lệnh qua queue cho TaskNeoPixel
      if (ledCommandQueue != nullptr) {
        xQueueSend(ledCommandQueue, &newState, 0);
      }
      ledState = newState;
    }
  }
  attributesChanged = true;
}