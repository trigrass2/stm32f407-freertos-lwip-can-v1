#ifndef __NET_COMMAND_LIST_H__
#define __NET_COMMAND_LIST_H__

#include "main.h"
#include "string.h"
#include "lwip/api.h"

#define NET_COMMAND_PARAMETER_MAX_NUM     10
typedef struct _netcommandformatstruct
{
	char* comm;                                          //the command in the data head
	uint16_t   pnum;                                       //the command parameter number
	char* pname[NET_COMMAND_PARAMETER_MAX_NUM];          //the parameter name pointer
	char* pvalue[NET_COMMAND_PARAMETER_MAX_NUM];         //the parameter value pointer for trict command match
	int (*callback)(struct netconn *,char*[],int);                      	 //command decode function
	int (*response)(struct netconn *,int);                             //command response
}NET_COMMAND_FORMATE_STRUCT;

extern const NET_COMMAND_FORMATE_STRUCT sNetCommandFormat[];
extern int NetCommandNumber(void);

#endif
