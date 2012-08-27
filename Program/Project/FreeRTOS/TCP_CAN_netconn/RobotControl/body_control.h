#ifndef   __BODY_CONTROL_H__
#define   __BODY_CONTROL_H__
#include "globalstruct.h"
#include "canfestival.h"

void body_control_thread(void *arg);
void CAN1Master_ConfigureNode(CO_Data* d);
void CAN1Master_heartbeatError(CO_Data* d,  UNS8 heartbeatID);
void CAN1Master_initialisation(CO_Data* d);
void CAN1Master_preOperational(CO_Data* d);
void CAN1Master_operational(CO_Data* d);
void CAN1Master_stopped(CO_Data* d);
void CAN1Master_SlaveBootup(CO_Data* d, UNS8 SlaveID);
void CAN1Master_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);

#endif
