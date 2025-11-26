#include "WiFiWebManager.h"
// FreeRTOS includes for background task
#include <FS.h>
#include <LittleFS.h>

WiFiWebManager* WiFiWebManager::instance = nullptr;

// HTML cho giao diện web
const char HTML_PAGE[] PROGMEM = R"rawliteral(

)rawliteral";

WiFiWebManager::WiFiWebManager() {
    instance = this;
    server = nullptr;
    serverTaskHandle = nullptr;
    isAPMode = false;
    connectTimeout = 10000;
    lastCheckTime = 0;

    // Khởi động LittleFS
    if (!LittleFS.begin(true)) {   // true = format nếu mount lỗi
        Serial.println("❌ LittleFS mount failed!");
    } else {
        Serial.println("✅ LittleFS mounted.");
    }
}

WiFiWebManager::~WiFiWebManager() {
    if(server) {
        delete server;
    }
    preferences.end();
}

// Static convenience API
bool WiFiWebManager::start(const char* apName, const char* apPassword, unsigned long timeout) {
    if(instance == nullptr) {
        instance = new WiFiWebManager();
    }
    return instance->begin(apName, apPassword, timeout);
}

void WiFiWebManager::loopTask() {
    if(instance) instance->loop();
}

void WiFiWebManager::OpenWeb(const char* apName, const char* apPassword) {
    if(instance == nullptr) {
        instance = new WiFiWebManager();
    }

    // Optionally override AP name/password if provided
    if(apName) instance->apName = String(apName);
    if(apPassword) instance->apPassword = String(apPassword);

    // Ensure Preferences is initialized so handlers that save settings work
    instance->preferences.begin("wifi-manager", false);

    // Force start the config portal (AP + webserver) regardless of WiFi state
    instance->startConfigPortal();
}

void WiFiWebManager::resetStatic() {
    if(instance) instance->reset();
}

bool WiFiWebManager::isConnectedStatic() {
    return instance ? instance->isConnected() : false;
}

String WiFiWebManager::getSSIDStatic() {
    return instance ? instance->getSSID() : String("");
}

String WiFiWebManager::getDeviceIDStatic() {
    return instance ? instance->getDeviceID() : String("");
}

String WiFiWebManager::getServerURLStatic() {
    return instance ? instance->getServerURL() : String("");
}

bool WiFiWebManager::begin(const char* apName, const char* apPassword, unsigned long timeout) {
    this->apName = String(apName);
    this->apPassword = String(apPassword);
    this->connectTimeout = timeout;
    
    preferences.begin("wifi-manager", false);
    
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║      ESP32 WiFi Manager v1.0          ║");
    Serial.println("╚════════════════════════════════════════╝");
    
    // Kiểm tra xem đã có WiFi lưu chưa
    if(loadCredentials()) {
        Serial.println("📂 Đã tìm thấy thông tin WiFi đã lưu");
        Serial.print("   SSID: ");
        Serial.println(savedSSID);
        Serial.print("   Đang kết nối");
        
        WiFi.mode(WIFI_STA);
        if(connectToWiFi(savedSSID, savedPassword)) {
            Serial.println(" ✅ Thành công!");
            Serial.print("   IP: ");
            Serial.println(WiFi.localIP());
            Serial.println("╚════════════════════════════════════════╝\n");
            return true;
        }
        Serial.println(" ❌ Thất bại!");
        Serial.println("   Khởi động chế độ cấu hình...");
    } else {
        Serial.println("📂 Chưa có thông tin WiFi");
        Serial.println("   Khởi động chế độ cấu hình...");
    }
    
    // Nếu không kết nối được, mở Access Point
    startConfigPortal();
    Serial.println("╚════════════════════════════════════════╝\n");
    return false;
}

void WiFiWebManager::loop() {
    // Xử lý web server nếu đang ở chế độ AP
    if(isAPMode && server) {
        server->handleClient();
        return;
    }
    
    // Kiểm tra kết nối WiFi định kỳ (mỗi 30 giây)
    if(millis() - lastCheckTime > 30000) {
        lastCheckTime = millis();
        
        if(WiFi.status() != WL_CONNECTED) {
            Serial.println("⚠️  Mất kết nối WiFi!");
            Serial.println("   Khởi động chế độ cấu hình...");
            startConfigPortal();
        }
    }
}

void WiFiWebManager::startConfigPortal() {
    if(isAPMode) return; // Đã ở chế độ AP rồi
    
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    
    bool hasPassword = (apPassword.length() >= 8);
    
    if(hasPassword) {
        WiFi.softAP(apName.c_str(), apPassword.c_str());
    } else {
        WiFi.softAP(apName.c_str());
    }
    
    IPAddress IP = WiFi.softAPIP();
    isAPMode = true;
    
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║   📡 Chế độ cấu hình WiFi đã khởi động ║");
    Serial.println("╠════════════════════════════════════════╣");
    Serial.print("║ WiFi: ");
    int spaces = 33 - apName.length();
    Serial.print(apName);
    for(int i=0; i<spaces; i++) Serial.print(" ");
    Serial.println("║");
    
    if(hasPassword) {
        Serial.print("║ Pass: ");
        spaces = 33 - apPassword.length();
        Serial.print(apPassword);
        for(int i=0; i<spaces; i++) Serial.print(" ");
        Serial.println("║");
    } else {
        Serial.println("║ Pass: (không có mật khẩu)             ║");
    }
    
    Serial.print("║ URL:  http://");
    String ipStr = IP.toString();
    spaces = 26 - ipStr.length();
    Serial.print(ipStr);
    for(int i=0; i<spaces; i++) Serial.print(" ");
    Serial.println("║");
    Serial.println("╠════════════════════════════════════════╣");
    Serial.println("║ 1. Kết nối điện thoại vào WiFi trên    ║");
    Serial.println("║ 2. Truy cập địa chỉ web                ║");
    Serial.println("║ 3. Chọn WiFi và nhập mật khẩu          ║");
    
    setupWebServer();
    // Start a background FreeRTOS task to handle client requests so
    // the server works without calling handleClient() from loop().
    if(serverTaskHandle == nullptr) {
        // create task pinned to core 1 (ESP32 default)
        xTaskCreatePinnedToCore(serverTask, "webServerTask", 4096, this, 1, (TaskHandle_t*)&serverTaskHandle, 1);
    }

    // Register WiFi disconnect event handler (event-driven fallback)
    WiFi.onEvent([](arduino_event_id_t event, arduino_event_info_t info){
        if(event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
            if(WiFiWebManager::instance && !WiFiWebManager::instance->isAPMode) {
                Serial.println("⚠️ [WiFiEvent] STA disconnected -> starting config portal");
                WiFiWebManager::instance->startConfigPortal();
            }
        }
    }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

void WiFiWebManager::stopConfigPortal() {
    if(!isAPMode) return;
    
    if(server) {
        delete server;
        server = nullptr;
    }
    
    // Stop background task if running
    if(serverTaskHandle != nullptr) {
        vTaskDelete((TaskHandle_t)serverTaskHandle);
        serverTaskHandle = nullptr;
    }

    // No monitor task here; event handler is used for disconnect fallback.

    WiFi.softAPdisconnect(true);
    isAPMode = false;
    
    Serial.println("✅ Tắt chế độ cấu hình");
}

void WiFiWebManager::setupWebServer() {
    if(server) delete server;
    
    server = new WebServer(80);
    server->on("/", staticHandleRoot);
    server->on("/scan", staticHandleScan);
    server->on("/connect", HTTP_POST, staticHandleConnect);
    server->on("/save_settings", HTTP_POST, staticHandleSaveSettings);
    server->on("/status", staticHandleStatus);
    server->on("/info", staticHandleInfo);
    server->begin();
}

bool WiFiWebManager::connectToWiFi(String ssid, String pass) {
    // Try multiple attempts with more verbose logging to help debug connection issues
    const int maxAttempts = 3;
    for(int attempt = 1; attempt <= maxAttempts; ++attempt) {
        Serial.print("\n[connectToWiFi] Attempt "); Serial.print(attempt); Serial.print(" to connect to '"); Serial.print(ssid); Serial.println("'");
        WiFi.begin(ssid.c_str(), pass.c_str());

        unsigned long startTime = millis();
        while(WiFi.status() != WL_CONNECTED && millis() - startTime < connectTimeout) {
            delay(200);
            Serial.print('.');
        }

        int status = WiFi.status();
        Serial.println();
        Serial.print("[connectToWiFi] WiFi.status() = "); Serial.println(status);

        if(status == WL_CONNECTED) {
            return true;
        }

        Serial.println("[connectToWiFi] Failed to connect on attempt " + String(attempt));
        // small delay before retrying
        delay(500);
    }

    return false;
}

void WiFiWebManager::saveCredentials(String ssid, String pass) {
    preferences.putString("ssid", ssid);
    preferences.putString("password", pass);
    savedSSID = ssid;
    savedPassword = pass;
}

bool WiFiWebManager::loadCredentials() {
    savedSSID = preferences.getString("ssid", "");
    savedPassword = preferences.getString("password", "");
    // Load optional settings
    savedDeviceID = preferences.getString("device_id", "");
    savedServerURL = preferences.getString("server_url", "");
    return savedSSID.length() > 0;
}

void WiFiWebManager::saveSettings(String deviceId, String serverUrl) {
    preferences.putString("device_id", deviceId);
    preferences.putString("server_url", serverUrl);
    savedDeviceID = deviceId;
    savedServerURL = serverUrl;
}

bool WiFiWebManager::loadSettings() {
    savedDeviceID = preferences.getString("device_id", "");
    savedServerURL = preferences.getString("server_url", "");
    return (savedDeviceID.length() > 0 || savedServerURL.length() > 0);
}

String WiFiWebManager::getDeviceID() { return savedDeviceID; }
String WiFiWebManager::getServerURL() { return savedServerURL; }

void WiFiWebManager::reset() {
    Serial.println("🔄 Reset thông tin WiFi...");
    preferences.clear();
    savedSSID = "";
    savedPassword = "";
    WiFi.disconnect();
    stopConfigPortal();
    startConfigPortal();
}

bool WiFiWebManager::isConnected() {
    return !isAPMode && WiFi.status() == WL_CONNECTED;
}

String WiFiWebManager::getSSID() {
    return savedSSID;
}

void WiFiWebManager::handleRoot() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        Serial.println("❌ Không mở được /index.html");
        server->send(500, "text/plain", "Loi: Khong mo duoc /index.html");
        return;
    }

    server->streamFile(file, "text/html");
    file.close();
}

void WiFiWebManager::handleScan() {
    int n = WiFi.scanNetworks();
    String json = "{\"networks\":[";
    
    for(int i = 0; i < n; i++) {
        if(i > 0) json += ",";
        json += "{";
        json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        json += "\"encryption\":\"" + String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "secured") + "\"";
        json += "}";
    }
    
    json += "]}";
    server->send(200, "application/json", json);
}

void WiFiWebManager::handleConnect() {
    String ssid = server->arg("ssid");
    String pass = server->arg("password");
    String deviceId = server->arg("device_id");
    String serverUrl = server->arg("server_url");

    // Server-side validation / sanitization
    deviceId.trim();
    serverUrl.trim();
    if(deviceId.length() > 128) {
        String json = "{\"success\":false,\"message\":\"Device ID quá dài (tối đa 128 ký tự)\"}";
        server->send(400, "application/json", json);
        return;
    }
    if(deviceId.length() > 0) {
        // allow letters, digits, dash, underscore, dot, colon
        bool ok = true;
        for(size_t i=0;i<deviceId.length();i++){
            char c = deviceId.charAt(i);
            if( !( (c>='0'&&c<='9') || (c>='A'&&c<='Z') || (c>='a'&&c<='z') || c=='-' || c=='_' || c=='.' || c==':' ) ) { ok = false; break; }
        }
        if(!ok) {
            String json = "{\"success\":false,\"message\":\"Device ID chứa ký tự không hợp lệ\"}";
            server->send(400, "application/json", json);
            return;
        }
    }
    if(serverUrl.length() > 0) {
        if(serverUrl.length() > 256) {
            String json = "{\"success\":false,\"message\":\"Server URL quá dài (tối đa 256 ký tự)\"}";
            server->send(400, "application/json", json);
            return;
        }
        if(!(serverUrl.startsWith("http://") || serverUrl.startsWith("https://"))) {
            String json = "{\"success\":false,\"message\":\"Server URL phải bắt đầu bằng http:// hoặc https://\"}";
            server->send(400, "application/json", json);
            return;
        }
    }

    Serial.println("\n📡 Đang kết nối WiFi...");
    Serial.print("   SSID: ");
    Serial.println(ssid);
    Serial.print("   ");

    // Tạm thời chuyển sang chế độ STA để kết nối
    WiFi.mode(WIFI_AP_STA);

    if(connectToWiFi(ssid, pass)) {
        // Save credentials and settings to Preferences
        saveCredentials(ssid, pass);
        saveSettings(deviceId, serverUrl);

        Serial.println(" ✅ Thành công!");
        Serial.print("   IP: ");
        Serial.println(WiFi.localIP());

        String json = "{\"success\":true,\"message\":\"Kết nối thành công\"}";
        server->send(200, "application/json", json);

        // Give browser time to receive response, then reboot to apply settings
        delay(1000);
        Serial.println("🔁 Khởi động lại thiết bị để áp dụng cấu hình...");
        ESP.restart();
    } else {
        Serial.println(" ❌ Thất bại!");
        WiFi.mode(WIFI_AP);

        String json = "{\"success\":false,\"message\":\"Không thể kết nối. Kiểm tra lại mật khẩu\"}";
        server->send(200, "application/json", json);
    }
}

void WiFiWebManager::handleSaveSettings() {
    String deviceId = server->arg("device_id");
    String serverUrl = server->arg("server_url");

    deviceId.trim();
    serverUrl.trim();

    // Basic validation (reuse the same rules)
    if(deviceId.length() > 128) {
        String json = "{\"success\":false,\"message\":\"Device ID quá dài (tối đa 128 ký tự)\"}";
        server->send(400, "application/json", json);
        return;
    }
    if(deviceId.length() > 0) {
        bool ok = true;
        for(size_t i=0;i<deviceId.length();i++){
            char c = deviceId.charAt(i);
            if( !( (c>='0'&&c<='9') || (c>='A'&&c<='Z') || (c>='a'&&c<='z') || c=='-' || c=='_' || c=='.' || c==':' ) ) { ok = false; break; }
        }
        if(!ok) {
            String json = "{\"success\":false,\"message\":\"Device ID chứa ký tự không hợp lệ\"}";
            server->send(400, "application/json", json);
            return;
        }
    }
    if(serverUrl.length() > 0) {
        if(serverUrl.length() > 256) {
            String json = "{\"success\":false,\"message\":\"Server URL quá dài (tối đa 256 ký tự)\"}";
            server->send(400, "application/json", json);
            return;
        }
        if(!(serverUrl.startsWith("http://") || serverUrl.startsWith("https://"))) {
            String json = "{\"success\":false,\"message\":\"Server URL phải bắt đầu bằng http:// hoặc https://\"}";
            server->send(400, "application/json", json);
            return;
        }
    }

    saveSettings(deviceId, serverUrl);

    String json = "{\"success\":true,\"message\":\"Lưu cấu hình thành công\"}";
    server->send(200, "application/json", json);
}

void WiFiWebManager::handleStatus() {
    String json = "{\"connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false") + "}";
    server->send(200, "application/json", json);
}

void WiFiWebManager::handleInfo() {
    String json = "{";
    json += "\"apName\":\"" + apName + "\",";
    json += "\"apIP\":\"" + WiFi.softAPIP().toString() + "\",";
    json += "\"device_id\":\"" + savedDeviceID + "\",";
    json += "\"server_url\":\"" + savedServerURL + "\"";
    json += "}";
    server->send(200, "application/json", json);
}

// Static handlers
void WiFiWebManager::staticHandleRoot() { instance->handleRoot(); }
void WiFiWebManager::staticHandleScan() { instance->handleScan(); }
void WiFiWebManager::staticHandleConnect() { instance->handleConnect(); }
void WiFiWebManager::staticHandleSaveSettings() { instance->handleSaveSettings(); }
void WiFiWebManager::staticHandleStatus() { instance->handleStatus(); }
void WiFiWebManager::staticHandleInfo() { instance->handleInfo(); }

// Background task: repeatedly call handleClient()
void WiFiWebManager::serverTask(void* param) {
    WiFiWebManager* self = reinterpret_cast<WiFiWebManager*>(param);
    if(!self) {
        vTaskDelete(NULL);
        return;
    }

    while(self->isAPMode) {
        if(self->server) self->server->handleClient();
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}

// (Removed) WiFi monitor task — replaced by WiFi event handler registration in startConfigPortal().