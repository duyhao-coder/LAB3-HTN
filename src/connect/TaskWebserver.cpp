#ifndef WEBDASHBOARD_H
#define WEBDASHBOARD_H
#include "globals.h"

class WebDashboard  {
private:
  WebServer server;
  float temperature;
  float humidity;
  
  const char* index_html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Dashboard</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        
        .container {
            width: 100%;
            max-width: 1200px;
        }
        
        .header {
            text-align: center;
            color: white;
            margin-bottom: 40px;
            animation: fadeInDown 0.8s ease;
        }
        
        .header h1 {
            font-size: 3em;
            font-weight: 700;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
            margin-bottom: 10px;
        }
        
        .header p {
            font-size: 1.2em;
            opacity: 0.9;
        }
        
        .dashboard {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 30px;
            animation: fadeInUp 0.8s ease;
        }
        
        .card {
            background: rgba(255, 255, 255, 0.95);
            border-radius: 25px;
            padding: 40px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
            position: relative;
            overflow: hidden;
        }
        
        .card::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 5px;
            background: linear-gradient(90deg, #667eea, #764ba2);
        }
        
        .card:hover {
            transform: translateY(-10px);
            box-shadow: 0 30px 80px rgba(0,0,0,0.4);
        }
        
        .icon-wrapper {
            width: 80px;
            height: 80px;
            border-radius: 20px;
            display: flex;
            align-items: center;
            justify-content: center;
            margin-bottom: 20px;
            font-size: 40px;
            animation: pulse 2s infinite;
        }
        
        .temp-icon {
            background: linear-gradient(135deg, #ff6b6b, #ee5a6f);
            color: white;
        }
        
        .humidity-icon {
            background: linear-gradient(135deg, #4facfe, #00f2fe);
            color: white;
        }
        
        .card-title {
            font-size: 1.4em;
            color: #555;
            margin-bottom: 15px;
            font-weight: 600;
        }
        
        .card-value {
            font-size: 3.5em;
            font-weight: 700;
            color: #333;
            margin-bottom: 10px;
            line-height: 1;
        }
        
        .card-unit {
            font-size: 1.8em;
            color: #999;
            font-weight: 300;
        }
        
        .update-time {
            text-align: center;
            color: white;
            margin-top: 30px;
            font-size: 1em;
            opacity: 0.8;
        }
        
        .status-indicator {
            display: inline-block;
            width: 12px;
            height: 12px;
            border-radius: 50%;
            background: #4ade80;
            margin-right: 8px;
            animation: blink 2s infinite;
        }
        
        @keyframes fadeInDown {
            from {
                opacity: 0;
                transform: translateY(-30px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        @keyframes fadeInUp {
            from {
                opacity: 0;
                transform: translateY(30px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.05);
            }
        }
        
        @keyframes blink {
            0%, 100% {
                opacity: 1;
            }
            50% {
                opacity: 0.3;
            }
        }
        
        @media (max-width: 768px) {
            .header h1 {
                font-size: 2em;
            }
            .card {
                padding: 30px;
            }
            .card-value {
                font-size: 2.5em;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🌡️ ESP32 Dashboard</h1>
            <p>Giám sát nhiệt độ & độ ẩm thời gian thực</p>
        </div>
        
        <div class="dashboard">
            <div class="card">
                <div class="icon-wrapper temp-icon">🌡️</div>
                <div class="card-title">Nhiệt Độ</div>
                <div class="card-value">
                    <span id="temperature">--</span>
                    <span class="card-unit">°C</span>
                </div>
            </div>
            
            <div class="card">
                <div class="icon-wrapper humidity-icon">💧</div>
                <div class="card-title">Độ Ẩm</div>
                <div class="card-value">
                    <span id="humidity">--</span>
                    <span class="card-unit">%</span>
                </div>
            </div>
        </div>
        
        <div class="update-time">
            <span class="status-indicator"></span>
            Cập nhật lần cuối: <span id="lastUpdate">--:--:--</span>
        </div>
    </div>
    
    <script>
        function updateData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('temperature').textContent = data.temperature.toFixed(1);
                    document.getElementById('humidity').textContent = data.humidity.toFixed(1);
                    
                    const now = new Date();
                    const timeString = now.toLocaleTimeString('vi-VN');
                    document.getElementById('lastUpdate').textContent = timeString;
                })
                .catch(error => console.error('Error:', error));
        }
        
        updateData();
        setInterval(updateData, 2000);
    </script>
</body>
</html>
)rawliteral";

  void handleRoot() {
    server.send(200, "text/html", index_html);
  }
  
  void handleData() {
    String json = "{";
    json += "\"temperature\":" + String(temperature, 1) + ",";
    json += "\"humidity\":" + String(humidity, 1);
    json += "}";
    
    server.send(200, "application/json", json);
  }

public:
  WebDashboard () : server(80) {
    temperature = 0.0;
    humidity = 0.0;
  }
  
  // Hàm khởi động web server - GỌI HÀM NÀY SAU KHI WIFI KẾT NỐI
  void begin() {
    server.on("/", [this]() { this->handleRoot(); });
    server.on("/data", [this]() { this->handleData(); });
    
    server.begin();
    Serial.println("✅ Web Dashboard đã khởi động!");
    Serial.print("🌐 Truy cập tại: http://");
    Serial.println(WiFi.localIP());
  }
  
  // Hàm cập nhật dữ liệu từ cảm biến
  void updateSensorData(float temp, float hum) {
    temperature = temp;
    humidity = hum;
  }
  
  // Hàm xử lý request - GỌI TRONG LOOP()
  void handleClient() {
    server.handleClient();
  }
};



#endif
