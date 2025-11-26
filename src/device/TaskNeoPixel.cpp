#include "TaskNeoPIXEL.h"
Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

// Định nghĩa màu sắc của các trạng thái
uint32_t colors[] = {
  strip.Color(255, 0, 0),     // Đỏ
  strip.Color(0, 255, 0),     // Xanh lá
  strip.Color(0, 0, 255),     // Xanh dương
  strip.Color(255, 0, 255),   // Tím
  strip.Color(255, 255, 255), // Trắng
  strip.Color(0, 0, 0)        // Tắt
};

void neo_blinky(void *pvParameters){
    strip.begin();
    // Set all pixels to off to start
    strip.clear();
    strip.show();

    int state = 0; // Khởi tạo trạng thái ban đầu
    Serial.begin(115200);  // Khởi tạo Serial để in log

    while(1) {   
        // Cập nhật màu sắc dựa trên trạng thái
        strip.setPixelColor(0, colors[state]); // Thiết lập màu cho pixel đầu tiên
        strip.show(); // Cập nhật dải đèn

        // In log khi LED sáng
        switch (state) {
            case 0:  // Trạng thái 1: Đỏ sáng 1s
                Serial.println("Trạng thái 1: Đỏ sáng 1 giây");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                break;
            case 1:  // Trạng thái 2: Xanh lá sáng 2s
                Serial.println("Trạng thái 2: Xanh lá sáng 2 giây");
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                break;
            case 2:  // Trạng thái 3: Xanh dương sáng 3s
                Serial.println("Trạng thái 3: Xanh dương sáng 3 giây");
                vTaskDelay(3000 / portTICK_PERIOD_MS);
                break;
            case 3:  // Trạng thái 4: Tím sáng 4s
                Serial.println("Trạng thái 4: Tím sáng 4 giây");
                vTaskDelay(4000 / portTICK_PERIOD_MS);
                break;
            case 4:  // Trạng thái 5: Trắng sáng 5s
                Serial.println("Trạng thái 5: Trắng sáng 5 giây");
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                break;
            case 5:  // Trạng thái 6: Tắt 1s
                Serial.println("Trạng thái 6: Tắt sáng 1 giây");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                break;
        }


        // Quay lại trạng thái đầu tiên sau khi hoàn thành trạng thái 6
        state = (state + 1) % 6;  // Sau khi hoàn thành trạng thái 6, quay lại trạng thái 1
    }
}
