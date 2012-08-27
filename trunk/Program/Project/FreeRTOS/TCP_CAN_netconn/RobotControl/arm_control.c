#include "arm_control.h"
#include "FreeRTOS.h"
#include "task.h"

void arm_control_thread(void *arg)
{
	NET_ROBOT_CONTROL_ARM_MESSAGE_STRUCT  arm_msg;      //���Ĵ���Ϣ
	NET_ROBOT_CONTROL_HAND_MESSAGE_STRUCT hand_msg;		   //�ڱ���Ϣ
	while(1)
	{
		//���ջ�е�ۿ�������
		if(0!=xQ_ARM_COMM && xQueueReceive( xQ_ARM_COMM, &(arm_msg), (portTickType)0))
		{
			printf("receieve a arm msg\r\n");
		}
		//���ջ�е�ֿ�������
		//if(0!=xQ_HAND_COMM && xQueueReceive( xQ_HAND_COMM, &(hand_msg), (portTickType)0))
		//{
		//	printf("receieve a hand msg\r\n");
		//}
			
		vTaskDelay(20);
	}
}
