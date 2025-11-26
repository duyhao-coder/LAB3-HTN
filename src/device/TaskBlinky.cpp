#include "TaskBlinky.h"

// Biến trạng thái của LED
LedState status = STATE_1;

void led_blinky(void *pvParameters){
  pinMode(LED_GPIO, OUTPUT);

  while(1) { 
    // Tùy thuộc vào trạng thái, đèn sẽ sáng và tắt theo thời gian quy định
    switch (status) {
      case STATE_1:
        Serial.println("STATE_1: LED ON for 1 second");
        digitalWrite(LED_GPIO, HIGH);  // Sáng LED
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Sáng 1 giây
        Serial.println("STATE_1: LED OFF for 1 second");
        digitalWrite(LED_GPIO, LOW);   // Tắt LED
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Tắt 1 giây
        break;

      case STATE_2:
        Serial.println("STATE_2: LED ON for 2 seconds");
        digitalWrite(LED_GPIO, HIGH);  // Sáng LED
        vTaskDelay(2000 / portTICK_PERIOD_MS);  // Sáng 2 giây
        Serial.println("STATE_2: LED OFF for 1 second");
        digitalWrite(LED_GPIO, LOW);   // Tắt LED
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Tắt 1 giây
        break;

      case STATE_3:
        Serial.println("STATE_3: LED ON for 3 seconds");
        digitalWrite(LED_GPIO, HIGH);  // Sáng LED
        vTaskDelay(3000 / portTICK_PERIOD_MS);  // Sáng 3 giây
        Serial.println("STATE_3: LED OFF for 1 second");
        digitalWrite(LED_GPIO, LOW);   // Tắt LED
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Tắt 1 giây
        break;

      case STATE_4:
        Serial.println("STATE_4: LED ON for 4 seconds");
        digitalWrite(LED_GPIO, HIGH);  // Sáng LED
        vTaskDelay(4000 / portTICK_PERIOD_MS);  // Sáng 4 giây
        Serial.println("STATE_4: LED OFF for 1 second");
        digitalWrite(LED_GPIO, LOW);   // Tắt LED
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Tắt 1 giây
        break;

      case STATE_5:
        Serial.println("STATE_5: LED ON for 5 seconds");
        digitalWrite(LED_GPIO, HIGH);  // Sáng LED
        vTaskDelay(5000 / portTICK_PERIOD_MS);  // Sáng 5 giây
        Serial.println("STATE_5: LED OFF for 1 second");
        digitalWrite(LED_GPIO, LOW);   // Tắt LED
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Tắt 1 giây
        break;

      default:
        break;
    }

    // Quay lại trạng thái đầu tiên sau khi hoàn thành STATE_5
    status = static_cast<LedState>((status % 5) + 1);  // Tăng trạng thái và quay lại trạng thái 1 khi tới STATE_5
  }
}
