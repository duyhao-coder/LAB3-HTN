#ifndef TASK_COREIOT_H
#define TASK_COREIOT_H

#include "globals.h"


void TaskCoreIoT(void *pvParameters);
void TaskTelemetry(void *pvParameters);
void TaskAttributes(void *pvParameters);
void TaskTbLoop(void *pvParameters);

#endif
