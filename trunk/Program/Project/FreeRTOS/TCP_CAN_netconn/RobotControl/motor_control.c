#include "motor_control.h"

/*
***********************  Motor_Init     **************************************************************
***********************  ��ʼ������ṹ��*************************************************************
***********************����ֵ   void     *************************************************************
***********************����              *************************************************************
***********************motor :   ����ṹ��ָ��*******************************************************
***********************  d   :   CANOpen�ṹ��ָ�� ***************************************************
***********************canNum:   CAN ��ڱ��      ***************************************************
*************************id  :   �����վID        ***************************************************
*************************pol :   �������ת����    ***************************************************
************************c_max��  ����������ֵ    ***************************************************
************************v_max��  ����ٶ����ֵ    ***************************************************
*/
void Motor_Init(MOTOR_STRUCT *motor,CO_Data *d,UNS8 canNum,UNS8 id,INTEGER8 pol,UNS16 c_max,UNS16 v_max)
{
    motor->p_od=d;
    motor->canBus=canNum;
    motor->nodeId=id;
    motor->controlword=0;
    motor->statusword=0;
    motor->mode_setting=MOTOR_NULL_MODE;
    motor->mode_actual=MOTOR_NULL_MODE;
    motor->current_setting=0;
    motor->current_actual=0;
    motor->velocity_setting=0;
    motor->velocity_actual=0;
    motor->position_setting=0;
    motor->position_actual=0;
    motor->current_max=c_max;
    motor->velocity_max=v_max;
    motor->operate_state=Unknown_state;
    motor->polarity=(-1==pol)?-1:1;
    motor->ErrorFlag=FALSE;
}
/*
************ ���õ�� ID ***********
*/
void SetMotorNodeId(MOTOR_STRUCT *motor,UNS8 id)
{
    motor->nodeId=id;
}
/*
************* ��ȡ���ID ***********
*/
UNS8 GetMotorNodeId(MOTOR_STRUCT *motor)
{
    return motor->nodeId;
}
/*
**************���ÿ�����************
************************
*/
void SetMotorControlWord(MOTOR_STRUCT *motor,UNS16 data)
{
    motor->controlword=data;
}
/*
**********   ��ȡ������****************
*/
UNS16 GetMotorControlWord(MOTOR_STRUCT *motor)
{
    return (motor->controlword);
}
/*
*********** ����Motor״̬��*************
*/
void SetMotorStatusWord(MOTOR_STRUCT *motor,UNS16 data)
{
    motor->statusword=data;
}
/*
*********** ��ȡ״̬�� *******************
*/
UNS16 GetMotorStatusWord(MOTOR_STRUCT *motor)
{
    return (motor->statusword);
}
/*
************ ����״̬��setting*******************
*/
void SetMotorModeSetting(MOTOR_STRUCT *motor,INTEGER8 mode)
{
	motor->mode_setting=mode;
}
/*
************ ���setting״ֵ̬******************
*/
INTEGER8 GetMotorModeSetting(MOTOR_STRUCT *motor)
{
    return (motor->mode_setting);
}
/*
************ ����״̬�� mode_actual***************
*/
void SetMotorModeActual(MOTOR_STRUCT *motor,INTEGER8 mode)
{
    motor->mode_actual=mode;
}
/*
*************��ȡ״̬��  mode_actual *************
*/
INTEGER8 GetMotorModeActual(MOTOR_STRUCT *motor)
{
    return (motor->mode_actual);
}
/*
*************���õ���ֵ*******************
*/
void SetMotorCurrentSetting(MOTOR_STRUCT *motor,INTEGER16 c_set)
{
	motor->current_setting=c_set*motor->polarity;	
}
/*
*************��ȡ currentsetting��ֵ
*/
INTEGER16 GetMotorCurrentSetting(MOTOR_STRUCT *motor)
{
    return (motor->current_setting*motor->polarity);
}
/*
*************���� current_actual��ֵ
*/
void SetMotorCurrentActual(MOTOR_STRUCT *motor,INTEGER16 c_actual)
{
    motor->current_actual=c_actual;
}
/*
*************��ȡ current_actual ��ֵ
*/
INTEGER16 GetMotorCurrentActual(MOTOR_STRUCT *motor)
{
    return (motor->current_actual*motor->polarity);
}
/*
**********���� velocity_settingֵ
*/
void SetMotorVelocitySetting(MOTOR_STRUCT *motor,INTEGER32 v_set)
{
    motor->velocity_setting=v_set*motor->polarity;
}
/*
***********��ȡ velocity_setting ֵ
*/
INTEGER16 GetMotorVelocitySetting(MOTOR_STRUCT *motor)
{
    return (motor->velocity_setting*motor->polarity);
}
/*
***********����  velocity_actual ֵ
*/
void SetMotorVelocityActual(MOTOR_STRUCT *motor,INTEGER32 v_actual)
{
    motor->velocity_actual=v_actual;
}
/*
***********��ȡ velocity_actual ֵ
*/
INTEGER32 GetMotorVelocityActual(MOTOR_STRUCT *motor)
{
    return (motor->velocity_actual*motor->polarity);
}

void SetMotorPositionSetting(MOTOR_STRUCT *motor,INTEGER32 p_set)
{
    motor->position_setting=p_set*motor->polarity;
}
/*
***********��ȡ position_setting ֵ
*/
INTEGER32 GetMotorPositionSetting(MOTOR_STRUCT *motor)
{
    return (motor->position_setting);
}
/*
***********����  position_actual ֵ
*/
void SetMotorPositionActual(MOTOR_STRUCT *motor,INTEGER32 p_actual)
{
    motor->position_actual=p_actual;
}
/*
***********��ȡ position_actual ֵ
*/
INTEGER32 GetMotorPositionActual(MOTOR_STRUCT *motor)
{
    return (motor->position_actual*motor->polarity);
}
/*
***********����  position_actual ֵ
*/
void SetMotorErrorCode(MOTOR_STRUCT *motor,UNS16 errCode,UNS8 errReg)
{
    motor->ErrorCode=errCode;
    motor->ErrorReg=errReg;
    motor->ErrorFlag=TRUE;
}
/*
***********���� operate_state
*/
void SetMotorOperateState(MOTOR_STRUCT *motor,e_nodeState state)
{
    switch(state)
    {
        case Initialisation:
        case Disconnected:
        case Preparing:
        case Stopped:
        case Operational:
        case Pre_operational:
            motor->operate_state=state;
            break;
        default:
            motor->operate_state=Unknown_state;
            break;
    }
}
/*
***********��ȡ operate_state ֵ
*/
e_nodeState GetMotorOperateState(MOTOR_STRUCT *motor)
{
    return (motor->operate_state);
}
/*
***********���� polarity
*/
void SetMotorPolarity(MOTOR_STRUCT *motor,INTEGER8 pol)
{
    motor->polarity=(-1==pol)?-1:1;
}
/*
***********��ȡ polarity ֵ
*/
INTEGER8 GetMotorPolarity(MOTOR_STRUCT *motor)
{
    return (motor->polarity);
}
/*
********** MotorSwitchON 
*/
void MotorSwitchON(MOTOR_STRUCT *motor)
{
	//Message p;

}
/*
********** MotorSwitchOFF
*/
void MotorSwitchOFF(MOTOR_STRUCT *motor)
{
    //Message p;
}
/*
********** MotorFaultReset
*/
void MotorFaultReset(MOTOR_STRUCT *motor)
{
    //Message p;
}
/*
********** MotorHomeStart
*/
void MotorHomeStart(MOTOR_STRUCT *motor)
{
    //Message p;
}
/*
********** MotorSetMode
*/
void MotorSetMode(MOTOR_STRUCT *motor,INTEGER8 mode)
{
    //Message p;
}
/*
********** MotorSetAutoPowerOFF
*/
void MotorSetAutoPowerOFF(MOTOR_STRUCT *motor,char en)
{
    //Message p;
}
/*
********** MotorSetPositionRel
*/
void MotorSetPositionRel(MOTOR_STRUCT *motor,char en)
{
    //Message p;
}
/*
********** MotorSetCurrent
*/
void MotorSetCurrent(MOTOR_STRUCT *motor,INTEGER16 c_set)
{
    //Message p;
}
/*
********** MotorSetVelocity
*/
void MotorSetVelocity(MOTOR_STRUCT *motor,INTEGER32 v_set)
{

}
/*
********** MotorSetCurrentNorm
*/
void MotorSetCurrentNorm(MOTOR_STRUCT *motor,INTEGER16 c_set,UNS16 norm)
{

}
/*
********** MotorSetVelocityNorm
*/
void MotorSetVelocityNorm(MOTOR_STRUCT *motor,INTEGER16 v_set,UNS16 norm)
{

}
/*
********** MotorSetPosition
*/
void MotorSetPosition(MOTOR_STRUCT *motor,INTEGER32 p_set)
{

}
/*
********** MotorSetHomeOffset
*/
UNS8 MotorSetHomeOffset(MOTOR_STRUCT *motor,INTEGER32 offset)
{
    return 0;//return (WriteSDO(0x607C,0x00,sizeof(INTEGER32),(void*)(&offset)));
}
/*
********** MotorGetHomeOffset
*/
UNS8 MotorGetHomeOffset(MOTOR_STRUCT *motor,INTEGER32 *offset)
{
    return 0;//return (ReadSDO(0x607C,0x00,sizeof(INTEGER32),(void*)(offset)));
}
/*
********** ���õ�����PID
*/
UNS8 MotorSetCurrentControlPar(MOTOR_STRUCT *motor,INTEGER16 p,INTEGER16 i,INTEGER16 d,UNS16 window)
{
	 return 0;
}
/*
********** �����ٶȻ�PID
*/
UNS8 MotorSetVelocityControlPar(MOTOR_STRUCT *motor,INTEGER16 p,INTEGER16 i,INTEGER16 d,UNS16 window)
{
	return 0;
}
/*
********** ����λ�û�PID
*/
UNS8 MotorSetPositionControlPar(MOTOR_STRUCT *motor,INTEGER16 p,INTEGER16 i,INTEGER16 d,UNS16 window)
{
	return 0;
}
/*
********** MotorSetCurrentCompensate
*/
UNS8 MotorSetCurrentCompensate(MOTOR_STRUCT *motor,INTEGER16 value)
{
    return 0;//(WriteSDO(0x60FB,0x04,sizeof(INTEGER16),(void*)(&value)));
}
/*
********** MotorSetSupplyVoltage
*/
void MotorSetSupplyVoltage(MOTOR_STRUCT *motor,UNS16 value)
{
    motor->supple_voltage=value;
}
/*
********** MotorGetMode
*/
INTEGER8 MotorGetMode(MOTOR_STRUCT *motor)
{
    return (motor->mode_actual);
}
/*
********** MotorGetCurrent
*/
INTEGER16 MotorGetCurrent(MOTOR_STRUCT *motor)
{
    return (motor->current_actual*motor->polarity);
}
/*
********** MotorGetVelocity
*/
INTEGER32 MotorGetVelocity(MOTOR_STRUCT *motor)
{
    return (motor->velocity_actual*motor->polarity);
}
/*
********** MotorGetPosition
*/
INTEGER32 MotorGetPosition(MOTOR_STRUCT *motor)
{
    return (motor->position_actual*motor->polarity);
}
/*
********** ��ȡ������PID
*/
UNS8 MotorGetCurrentControlPar(MOTOR_STRUCT *motor,INTEGER16 *p,INTEGER16 *i,INTEGER16 *d,UNS16 *window)
{
 	return 0;
}
/*
********** ��ȡ�ٶȻ�PID
*/
UNS8 MotorGetVelocityControlPar(MOTOR_STRUCT *motor,INTEGER16 *p,INTEGER16 *i,INTEGER16 *d,UNS16 *window)
{
	return 0;
}
/*
********** ��ȡλ�û�PID
*/
UNS8 MotorGetPositionControlPar(MOTOR_STRUCT *motor,INTEGER16 *p,INTEGER16 *i,INTEGER16 *d,UNS16 *window)
{
	return 0;
}
/*
********** MotorGetCurrentCompensate
*/
UNS8 MotorGetCurrentCompensate(MOTOR_STRUCT *motor,INTEGER16 *value)
{
    return 0;//(ReadSDO(0x60FB,0x04,sizeof(INTEGER16),(void*)(&value)));
}
/*
********** MotorGetDCVoltage
*/
UNS8 MotorGetDCVoltage(MOTOR_STRUCT *motor,UNS32 *voltage)
{
    return 0;//(ReadSDO(0x6079,0x00,sizeof(UNS32),(void*)(voltage)));
}
/*
********** MotorGetSuppleVoltage
*/
UNS16 MotorGetSuppleVoltage(MOTOR_STRUCT *motor)
{
    return motor->supple_voltage;
}
/*
********** MotorGetErrorCode
*/
char MotorGetErrorCode(MOTOR_STRUCT *motor,UNS16 *errCode,UNS8 *errReg)
{
    if(TRUE==motor->ErrorFlag)
    {
        *errCode=motor->ErrorCode;
        *errReg=motor->ErrorReg;
        return TRUE;
    }
    return FALSE;

}
/*
********** MotorIsTargetReach
*/
char MotorIsTargetReach(MOTOR_STRUCT *motor)
{
    return ((motor->statusword&STATUSWORD_TAGREACH)?TRUE:FALSE);
}
/*
********** MotorIsSwitchON
*/
char MotorIsSwitchON(MOTOR_STRUCT *motor)
{
    return (((STATUSWORD_R_SW_ON|STATUSWORD_SW_ON|STATUSWORD_OP_EN)==(motor->statusword&(STATUSWORD_R_SW_ON|STATUSWORD_SW_ON|STATUSWORD_OP_EN)))?TRUE:FALSE);
}
/*
********** MotorIsSwitchOFF
*/
char MotorIsSwitchOFF(MOTOR_STRUCT *motor)
{
    return ((0==(motor->statusword&(STATUSWORD_R_SW_ON|STATUSWORD_SW_ON|STATUSWORD_OP_EN)))?TRUE:FALSE);
}
/*
********** MotorIsAutoPowerOFF
*/
char MotorIsAutoPowerOFF(MOTOR_STRUCT *motor)
{
    return ((motor->controlword&CONTROL_I_AUTO_CLEAR)?TRUE:FALSE);
}
/*
********** MotorIsPositionRel
*/
char MotorIsPositionRel(MOTOR_STRUCT *motor)
{
    return ((motor->controlword&CONTROLWORD_PP_REL)?TRUE:FALSE);
}
/*
********** MotorIsFault
*/
char MotorIsFault(MOTOR_STRUCT *motor)
{
    return motor->ErrorFlag;
}

void MotorControl_SDOWriteCallback(CO_Data* d, UNS8 nodeId)
{

}
void MotorControl_SDOReadCallback(CO_Data* d, UNS8 nodeId)
{

}

