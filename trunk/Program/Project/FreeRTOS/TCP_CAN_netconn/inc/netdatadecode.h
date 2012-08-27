#ifndef __NETDATA_DECODE_H__
#define __NETDATA_DECODE_H__

#include "NetCommandList.h"
#include "lwip/api.h"

int NetDataDecode(struct netconn *sock,char* data);

int REG_CommandDecode(struct netconn *sock,char* pvalue[]);
int DRIVE_DirectVelocityDecode(struct netconn *sock,char* pvalue[],int command_number);
int DRIVE_DirectCurrentDecode(struct netconn *sock,char* pvalue[],int command_number);
int DRIVE_IndirectDecode(struct netconn *sock,char* pvalue[],int command_number);
int GO_AheadDecode(struct netconn *sock,char* pvalue[],int command_number);
int GO_RotateDecode(struct netconn *sock,char* pvalue[],int command_number);
int comm_CommandResponse(struct netconn *sock,int state);

#endif
