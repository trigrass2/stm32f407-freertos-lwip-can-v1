#include "robot_control.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "globalstruct.h"
#include "body_control.h"
#include "arm_control.h"
#include "canfestival.h"
#include "can_STM32.h"
#include "canopen_thread.h"
///////����������ֵ�Ĳ�ͬ����ͬ
//#include "ObjDict_CAN1.h"
//#include "ObjDict_CAN1.h"

xTaskHandle xH_Body,xH_Arm,xH_Robot;
//extern CO_DATA_STRUCT  CO_D;

static void motion_control_thread(void *arg)
{
	initTimer();                               //��ʼ��CANopen��ʱ��
	if(1 == ROBOT_BODY)                        //����������ƽ���
	{
		CO_D.CO_CAN1 = &ObjDict_CAN1_Data;
		CO_D.CO_CAN1->canHandle = CAN1;
		                                                        //�������Ĵ���Ϣ����
		xQ_DRIVE_COMM = xQueueCreate( 5, sizeof(NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT));
		                                                        //�����ڱ���Ϣ����
		xQ_FLIP_COMM = xQueueCreate( 5, sizeof(NET_ROBOT_CONTROL_FLIPPER_MESSAGE_STRUCT));
		                                                        //����������ƽ���
		xTaskCreate(body_control_thread, "body_control", BODY_CONTROL_THREAD_STACK, CO_D.CO_CAN1,BODY_CONTROL_THREAD_PRIO, &xH_Body);
		if(NULL == xH_Body)
		{
			printf("!!body control thread create error \r\n");
		}
		//else
			//printf("body control thread created\r\n");
			
	}
	if(1 == ROBOT_WITH_ARM)                           //������е�ֿ��ƽ���
	{
		canInit(CAN2,CAN_BAUD_1M);                   //��ʼ��CAN2
		////////
		CO_D.CO_CAN2 = &ObjDict_CAN2_Data;           //���ṹ��ָ���ַ��������ֵ����ֲ�ͬ����ͬ
		//������������Ϣ����
		xQ_ARM_COMM = xQueueCreate( 5, sizeof(NET_ROBOT_CONTROL_ARM_MESSAGE_STRUCT) );
		//������е����Ϣ����
		xQ_HAND_COMM = xQueueCreate( 5, sizeof(NET_ROBOT_CONTROL_HAND_MESSAGE_STRUCT) );
		//������е�ֿ��ƽ���
		xTaskCreate(arm_control_thread, "arm_control", ARM_CONTROL_THREAD_STACK, CO_D.CO_CAN2,ARM_CONTROL_THREAD_PRIO, &xH_Arm);
		if(NULL == xH_Arm)
		{
			printf("!!arm control thread create error \r\n");         //�������̳���
		}
		else
			printf("arm control thread created\r\n");
	}
	
	while(1)
	{
		vTaskDelay(1000);
		//vTaskSuspend( NULL );                  //�����Լ�
	}	
}
void start_robot_control(void)
{
	xTaskCreate(motion_control_thread, "motion_control", configMINIMAL_STACK_SIZE, NULL,MOTION_THREAD_PRIO, &xH_Robot);
	if(NULL == xH_Robot)
	{
	 	printf("motion control thread creat failed!\r\n");
	}	
}
