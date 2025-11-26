#ifndef __TINY_ML__
#define __TINY_ML__


#include "globals.h"
#include "dht_anomaly_model.h"


void setupTinyML();
void tiny_ml_task(void *pvParameters);
void generateRandomData();
#endif