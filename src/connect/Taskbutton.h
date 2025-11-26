#ifndef TASK_BUTTON_H
#define TASK_BUTTON_H

#include "globals.h"



#define BTN_CONFIG_PIN 0        // GPIO0 = nút BOOT trên YOLO UNO

void TaskButtonConfig(void *pvParameters);   // task mới đọc nút
void stopTasksIoT();
#endif
