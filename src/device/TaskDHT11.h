#ifndef INC_TASKDHT11_H_
#define INC_TASKDHT11_H_

#include "globals.h"
#define DHTPIN 1       // Chân DATA của DHT11 (chỉnh theo mạch thật)
#define DHTTYPE DHT11   // hoặc DHT22 nếu dùng loại đó

// ⏱️ Thời gian delay cho mỗi task
#define TEMP_DELAY 2000
#define HUMI_DELAY 2000
void InitDHT11();
float getTemperature();
float getHumidity();

// (Tuỳ chọn) Task nếu muốn chạy song song
void TaskTemperature(void *pvParameters);
void TaskHumidity(void *pvParameters);
#endif /* INC_TASKDHT11_H_ */