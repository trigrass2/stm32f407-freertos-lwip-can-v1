#include "arm_control.h"
#include "FreeRTOS.h"
#include "task.h"

void arm_control_thread(void *arg)
{
	NET_ROBOT_CONTROL_ARM_MESSAGE_STRUCT  arm_msg;      //主履带消息
	NET_ROBOT_CONTROL_HAND_MESSAGE_STRUCT hand_msg;		   //摆臂消息
	while(1)
	{
		//接收机械臂控制命令
		if(0!=xQ_ARM_COMM && xQueueReceive( xQ_ARM_COMM, &(arm_msg), (portTickType)0))
		{
			printf("receieve a arm msg\r\n");
		}
		//接收机械手控制命令
		//if(0!=xQ_HAND_COMM && xQueueReceive( xQ_HAND_COMM, &(hand_msg), (portTickType)0))
		//{
		//	printf("receieve a hand msg\r\n");
		//}
			
		vTaskDelay(20);
	}
}
