#include "body_control.h"
#include "FreeRTOS.h"
#include "task.h"
#include "canfestival.h"
#include "can_STM32.h"

void body_control_thread(void *arg)
{
	NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT  body_msg;      //主履带消息
	NET_ROBOT_CONTROL_FLIPPER_MESSAGE_STRUCT flip_msg;		   //摆臂消息
	CO_Data *d;
	d = (CO_Data *)arg;
	CAN1Master_ConfigureNode(d);           //配置节点
	canInit(CAN1,CAN_BAUD_1M);             //初始化CAN1
	printf(" start the body control\r\n");
	while(1)
	{
		//接收本体控制命令
		if(0!=xQ_DRIVE_COMM && xQueueReceive( xQ_DRIVE_COMM, &(body_msg), (portTickType)0))
		{
			printf("receieve a body msg\r\n");
		}
		//接收摆臂控制命令
		if(0!=xQ_FLIP_COMM && xQueueReceive( xQ_FLIP_COMM, &(flip_msg), (portTickType)0))
		{
			printf("receieve a flip msg\r\n");
		}
		//(void)masterSendNMTstateChange(d, 0, NMT_Reset_Node);	
		vTaskDelay(4000);
	}
}

void CAN1Master_ConfigureNode(CO_Data* d)
{
	  d->heartbeatError   = CAN1Master_heartbeatError;
	  d->initialisation   = CAN1Master_initialisation;
	  d->preOperational   = CAN1Master_preOperational;
	  d->operational      = CAN1Master_operational;
	  d->stopped	      = CAN1Master_stopped;
	  d->post_SlaveBootup = CAN1Master_SlaveBootup;
	  d->post_emcy        = CAN1Master_emcy;
	  setState(d,Initialisation);
}

void CAN1Master_heartbeatError(CO_Data* d,  UNS8 heartbeatID)
{
	d->NMTable[heartbeatID]=Unknown_state;
    printf(" HeartBeat not received from node : %d \r\n",heartbeatID);
}
void CAN1Master_initialisation(CO_Data* d)
{
  	printf(" CAN1 Entering in INIT \r\n");
}
void CAN1Master_preOperational(CO_Data* d)
{
	printf(" CAN1 Entering in PRE-OPERATIONAL \r\n");
	(void)masterSendNMTstateChange(d, 0, NMT_Reset_Node);
	//BodyMaster_boottimeInit(d);  //start a timer f or boot time
}
void CAN1Master_operational(CO_Data* d)
{
 	printf(" CAN1 Entering in OPERATIONAL \r\n");
	 
}
void CAN1Master_stopped(CO_Data* d)
{
    printf(" CAN1 Entering in STOPPED \r\n");
}
void CAN1Master_SlaveBootup(CO_Data* d, UNS8 SlaveID)
{
    printf(" receive Bootup SlaveId:%d \r\n",SlaveID);
	(void)masterSendNMTstateChange (d, SlaveID, NMT_Start_Node);
}
void CAN1Master_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg)
{
    printf(" there is node in emcy state\r\n");
}
