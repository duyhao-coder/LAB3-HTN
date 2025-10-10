// #include "TaskDHT20.h"
// #define I2C_SDA 11
// #define I2C_SCL 12

// DFRobot_DHT20 dht20;

// void InitDHT20() {
//     Wire.begin(I2C_SDA, I2C_SCL);
//     dht20.begin();
//     Serial.println("DHT20 initialized successfully.");
//     xTaskCreate(TaskTemperature, "TaskTemperature", 2048, NULL, 1, NULL);
//     xTaskCreate(TaskHumidity, "TaskHumidity", 2048, NULL, 1, NULL);
// }

// void TaskTemperature(void *pvParameters) {
//     while (true)
//     {
//         vTaskDelay(TEMP_DELAY / portTICK_PERIOD_MS);
//     }
// }

// void TaskHumidity(void *pvParameters) {
//     while (true)
//     {
//         vTaskDelay(HUMI_DELAY / portTICK_PERIOD_MS);
//     }
// }

// float getTemperature() {
//     if(!isnan(dht20.getTemperature())){
//         return dht20.getTemperature();
//     }
//     return -1; // Return an error value
// }

// float getHumidity() {
//     if(!isnan(dht20.getHumidity())){ 
//         return dht20.getHumidity();
//     }
//     return -1; // Return an error value

// }