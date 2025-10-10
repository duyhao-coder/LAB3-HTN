#ifndef GLOBALS_H
#define GLOBALS_H


// include libraries
#include <Wire.h>
#include <WiFi.h>
#include <DHT.h>
#include <DFRobot_DHT20.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <Ultrasonic.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <ElegantOTA.h>
#include <ESP_Google_Sheet_Client.h>
#include "time.h"
#include <GS_SDHelper.h> // For SD/SD_MMC mounting helper
#include <HTTPClient.h>
#include "WiFiClientSecure.h"
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include "DHT20.h"
#include <freertos/queue.h>
// #include "../lib/ThingsBoard/ThingsBoard.h"

//
#include "../src/connect/TaskWifi.h"
#include "../src/connect/Thingsboard.h"
#include "../src/connect/TaskCoreIoT.h"
#include "../src/device/TaskDHT11.h"
#include "../src/device/TaskNeoPixel.h"
#include "../src/device/TaskDHT20.h"





// delay times for tasks
#define LCD_DELAY 5000
#define LIGHT_SENSOR_DELAY 5000
#define SOIL_MOS_DELAY 5000
#define TEMP_DELAY 5000
#define HUMI_DELAY 5000
#define RGB_DELAY 2000
#define CONFIG_DELAY 100
#define CONNECT_DELAY 500
#define MOTOR_DELAY 1000
#define RELAY_DELAY 1000
#define ULTRASONIC_DELAY 5000
#define AP_DELAY 100

// include task chung
constexpr char WIFI_SSID[] = "Hao_Thai";
constexpr char WIFI_PASSWORD[] = "h@oth@i30082002";
constexpr char TOKEN[] = "C1rAuJkBteiOfLKELLmH";
constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
extern WiFiClient wifiClient;
extern Arduino_MQTT_Client mqttClient;
extern ThingsBoard tb;
#define LED_PIN     45     // NeoPixel built-in GPIO45
#define LED_COUNT   1      // số lượng LED
#define BUTTON_PIN  0      // nút nhấn GPIO0
#define TASK_DELAY  50     // thời gian quét nút (ms)
extern volatile bool attributesChanged;
extern volatile bool ledState;
extern volatile uint16_t blinkingInterval;
extern QueueHandle_t ledCommandQueue;


//
extern bool rainbowMode;
#endif