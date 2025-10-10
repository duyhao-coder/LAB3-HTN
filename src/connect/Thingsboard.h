#ifndef THINGSBOARD_H
#define THINGSBOARD_H

#include "globals.h"



RPC_Response setLedSwitchState(const RPC_Data &data);
void processSharedAttributes(const Shared_Attribute_Data &data);

bool connectCoreIoT();
void sendTelemetryData();
void sendAttributesData();
#endif
