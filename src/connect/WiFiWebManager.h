#ifndef WIFI_WEB_MANAGER_H
#define WIFI_WEB_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "globals.h"
class WiFiWebManager {
private:
    WebServer* server;
    Preferences preferences;
    String savedSSID;
    String savedPassword;
    String savedDeviceID;
    String savedServerURL;
    String apName;
    String apPassword;
    bool isAPMode;
    unsigned long connectTimeout;
    unsigned long lastCheckTime;
    
    static WiFiWebManager* instance;
    
    void startConfigPortal();
    void stopConfigPortal();
    void setupWebServer();
    bool connectToWiFi(String ssid, String pass);
    void saveCredentials(String ssid, String pass);
    void saveSettings(String deviceId, String serverUrl);
    bool loadCredentials();
    bool loadSettings();
    void handleRoot();
    void handleScan();
    void handleConnect();
    void handleSaveSettings();
    void handleStatus();
    void handleInfo();
    
    static void staticHandleRoot();
    static void staticHandleScan();
    static void staticHandleConnect();
    static void staticHandleSaveSettings();
    static void staticHandleStatus();
    static void staticHandleInfo();
    static void serverTask(void* param);

    // Task handle for background web server
    void* serverTaskHandle;
    // (Event-driven) WiFi disconnect handler will be registered instead of a monitor task.

    // Getters for saved settings
    String getDeviceID();
    String getServerURL();

public:
    WiFiWebManager();
    ~WiFiWebManager();
    
    // Hàm chính - gọi trong setup()
    // apName: Tên WiFi khi ESP32 ở chế độ cấu hình
    // apPassword: Mật khẩu (tối thiểu 8 ký tự, để trống = không mật khẩu)
    // timeout: Thời gian chờ kết nối WiFi (ms)
    bool begin(const char* apName = "ESP32-WiFi-Setup", const char* apPassword = "12345678", unsigned long timeout = 10000);
    
    // Xử lý các request từ web và kiểm tra kết nối
    void loop();
    
    // Reset thông tin WiFi đã lưu và khởi động lại chế độ cấu hình
    void reset();
    
    // Kiểm tra trạng thái kết nối WiFi
    bool isConnected();
    
    // Lấy SSID đang kết nối
    String getSSID();

    // Static/simple API so user doesn't need to instantiate object manually.
    // Call `WiFiWebManager::start(...)` in setup() and optionally
    // `WiFiWebManager::loopTask()` in loop().
    static bool start(const char* apName = "ESP32-Setup", const char* apPassword = "12345678", unsigned long timeout = 10000);
    static void loopTask();
    static void resetStatic();
    static bool isConnectedStatic();
    static String getSSIDStatic();
    // Static accessors for saved settings
    static String getDeviceIDStatic();
    static String getServerURLStatic();
    // Force-open the web configuration portal (start AP + webserver) immediately,
    // bypassing any checks about stored WiFi credentials. Optional apName/apPassword
    // override the previously configured AP name/password.
    static void OpenWeb(const char* apName = nullptr, const char* apPassword = nullptr);
};

#endif