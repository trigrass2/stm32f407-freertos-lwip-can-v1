#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#include "canfestival.h"

//Control word
#define CONTROLWORD_HALT             0x0100 //1:halt
#define CONTROLWORD_FAULT_RESET      0x0080 //1:fault reset
#define CONTROLWORD_PP_REL           0x0040 //0:Target position is an absolute value,1:Target position is a relative value
#define CONTROLWORD_SET_IMM          0x0020 //0:finish the actual position and then start next positioning,1:interrupt the actual positioning and start the next positioning
#define CONTROLWORD_NEW_SET_POINT    0x0010 //0:does not assume target position,1:assume target position
#define CONTROL_I_AUTO_CLEAR         0x0020  //auto clear current to 0 when target velocity is zero
#define CONTROLWORD_HOM_OPE_START    0x0008 //1:enable operation
#define CONTROLWORD_OPERATION_EN     0x0008 //1:enable operation
#define CONTROLWORD_QUIICK_STOP      0x0004 //1:quick stop
#define CONTROLWORD_ENABLE_VOL       0x0002 //1:enable voltage
#define CONTROLWORD_SWITCH_ON        0x0001 //1:switch on
#define CONTROLWORD_HOME_START       0x0010 //1:home start
//Status word
#define STATUSWORD_PRTOHOME          0x8000 //1:position referenced to home position
#define STATUSWORD_REF_CP            0x4000 //1:refresh cycle of power stage
#define STATUSWORD_PP_F_ERR          0x2000 //1:following error in position mode
#define STATUSWORD_HM_H_ERR          0x2000 //1:home error in home mode
#define STATUSWORD_PP_S_PA           0x1000 //1:set point ack in profile position mode
#define STATUSWORD_PV_SPEED          0x1000 //1:speed in profile velocity mode
#define STATUSWORD_HM_TARGR          0x1000 //1:homing attained in home mode
#define STATUSWORD_IN_LIM            0x0800 //1:internal limit active
#define STATUSWORD_TAGREACH          0x0400 //1:target reached
#define STATUSWORD_REMOTE            0x0200 //1:remote(NMT slave state operational)
#define STATUSWORD_OFF_C             0x0100 //1:offset current measured
#define STATUSWORD_NOT_USED          0x0080 //not used(warning)
#define STATUSWORD_SW_ON_DIS         0x0040 //switch on disable
#define STATUSWORD_QUI_STOP          0x0020 //quick stop
#define STATUSWORD_VOL_EN            0x0010 //voltage enalbed(power stage on)
#define STATUSWORD_FAULT             0x0008 //fault
#define STATUSWORD_OP_EN             0x0004 //operation enable
#define STATUSWORD_SW_ON             0x0002 //switch on
#define STATUSWORD_R_SW_ON           0x0001 //Ready to switch on

//Motor mode电机工作模式
#define MOTOR_H_MODE         4    //homing mode
#define MOTOR_PV_MODE        2    //profile velocity mode
//#define MOTOR_PP_MODE        1  //profile position mode
#define MOTOR_NULL_MODE      0    //not used mode
#define MOTOR_P_MODE         -1   //position mode
#define MOTOR_V_MODE         1    //velocity mode
#define MOTOR_C_MODE         -3   //current mode

typedef struct _struct_motor{
    CO_Data *p_od;
    UNS8 canBus;
    UNS8 nodeId;
    UNS16 controlword;
    UNS16 statusword;
    INTEGER8 mode_setting;
    INTEGER8 mode_actual;
    INTEGER8 polarity;
    INTEGER16 current_setting;
    INTEGER16 current_actual;
    INTEGER32 velocity_setting;
    INTEGER32 velocity_actual;
    INTEGER32 position_setting;
    INTEGER32 position_actual;
    UNS16 current_max;
    UNS16 velocity_max;
    UNS16 supple_voltage;
    e_nodeState operate_state;
    UNS16 ErrorCode;
    UNS8 ErrorReg;
    char ErrorFlag;
}MOTOR_STRUCT;

void Motor_Init(MOTOR_STRUCT *motor,CO_Data *d,UNS8 canNum,UNS8 id,INTEGER8 pol,UNS16 c_max,UNS16 v_max);
void SetMotorNodeId(MOTOR_STRUCT *motor,UNS8 id);
UNS8 GetMotorNodeId(MOTOR_STRUCT *motor);
UNS16 GetMotorControlWord(MOTOR_STRUCT *motor);
void SetMotorStatusWord(MOTOR_STRUCT *motor,UNS16 data);
UNS16 GetMotorStatusWord(MOTOR_STRUCT *motor);
void SetMotorModeSetting(MOTOR_STRUCT *motor,INTEGER8 mode);
INTEGER8 GetMotorModeSetting(MOTOR_STRUCT *motor);
void SetMotorModeActual(MOTOR_STRUCT *motor,INTEGER8 mode);
INTEGER8 GetMotorModeActual(MOTOR_STRUCT *motor);
void SetMotorCurrentSetting(MOTOR_STRUCT *motor,INTEGER16 c_set);
INTEGER16 GetMotorCurrentSetting(MOTOR_STRUCT *motor);
void SetMotorCurrentActual(MOTOR_STRUCT *motor,INTEGER16 c_actual);
INTEGER16 GetMotorCurrentActual(MOTOR_STRUCT *motor);
void SetMotorVelocitySetting(MOTOR_STRUCT *motor,INTEGER32 v_set);
INTEGER16 GetMotorVelocitySetting(MOTOR_STRUCT *motor);
void SetMotorVelocityActual(MOTOR_STRUCT *motor,INTEGER32 v_actual);
INTEGER32 GetMotorVelocityActual(MOTOR_STRUCT *motor);
void SetMotorPositionSetting(MOTOR_STRUCT *motor,INTEGER32 p_set);
INTEGER32 GetMotorPositionSetting(MOTOR_STRUCT *motor);
void SetMotorPositionActual(MOTOR_STRUCT *motor,INTEGER32 p_actual);
INTEGER32 GetMotorPositionActual(MOTOR_STRUCT *motor);
void SetMotorErrorCode(MOTOR_STRUCT *motor,UNS16 errCode,UNS8 errReg);
void SetMotorOperateState(MOTOR_STRUCT *motor,e_nodeState state);
e_nodeState GetMotorOperateState(MOTOR_STRUCT *motor);
void SetMotorPolarity(MOTOR_STRUCT *motor,INTEGER8 pol);
INTEGER8 GetMotorPolarity(MOTOR_STRUCT *motor);
void MotorSwitchON(MOTOR_STRUCT *motor);
void MotorSwitchOFF(MOTOR_STRUCT *motor);
void MotorFaultReset(MOTOR_STRUCT *motor);
void MotorHomeStart(MOTOR_STRUCT *motor);
void MotorSetMode(MOTOR_STRUCT *motor,INTEGER8 mode);
void MotorSetAutoPowerOFF(MOTOR_STRUCT *motor,char en);
void MotorSetPositionRel(MOTOR_STRUCT *motor,char en);
void MotorSetCurrent(MOTOR_STRUCT *motor,INTEGER16 c_set);
void MotorSetVelocity(MOTOR_STRUCT *motor,INTEGER32 v_set);
void MotorSetCurrentNorm(MOTOR_STRUCT *motor,INTEGER16 c_set,UNS16 norm);
void MotorSetVelocityNorm(MOTOR_STRUCT *motor,INTEGER16 v_set,UNS16 norm);
void MotorSetPosition(MOTOR_STRUCT *motor,INTEGER32 p_set);
UNS8 MotorSetHomeOffset(MOTOR_STRUCT *motor,INTEGER32 offset);
UNS8 MotorGetHomeOffset(MOTOR_STRUCT *motor,INTEGER32 *offset);
UNS8 MotorSetCurrentControlPar(MOTOR_STRUCT *motor,INTEGER16 p,INTEGER16 i,INTEGER16 d,UNS16 window);
UNS8 MotorSetVelocityControlPar(MOTOR_STRUCT *motor,INTEGER16 p,INTEGER16 i,INTEGER16 d,UNS16 window);
UNS8 MotorSetPositionControlPar(MOTOR_STRUCT *motor,INTEGER16 p,INTEGER16 i,INTEGER16 d,UNS16 window);
UNS8 MotorSetCurrentCompensate(MOTOR_STRUCT *motor,INTEGER16 value);
void MotorSetSupplyVoltage(MOTOR_STRUCT *motor,UNS16 value);
INTEGER8 MotorGetMode(MOTOR_STRUCT *motor);
INTEGER16 MotorGetCurrent(MOTOR_STRUCT *motor);
INTEGER32 MotorGetVelocity(MOTOR_STRUCT *motor);
INTEGER32 MotorGetPosition(MOTOR_STRUCT *motor);
UNS8 MotorGetCurrentControlPar(MOTOR_STRUCT *motor,INTEGER16 *p,INTEGER16 *i,INTEGER16 *d,UNS16 *window);
UNS8 MotorGetVelocityControlPar(MOTOR_STRUCT *motor,INTEGER16 *p,INTEGER16 *i,INTEGER16 *d,UNS16 *window);
UNS8 MotorGetPositionControlPar(MOTOR_STRUCT *motor,INTEGER16 *p,INTEGER16 *i,INTEGER16 *d,UNS16 *window);
UNS8 MotorGetCurrentCompensate(MOTOR_STRUCT *motor,INTEGER16 *value);
UNS8 MotorGetDCVoltage(MOTOR_STRUCT *motor,UNS32 *voltage);
UNS16 MotorGetSuppleVoltage(MOTOR_STRUCT *motor);
char MotorGetErrorCode(MOTOR_STRUCT *motor,UNS16 *errCode,UNS8 *errReg);
char MotorIsTargetReach(MOTOR_STRUCT *motor);
char MotorIsSwitchON(MOTOR_STRUCT *motor);
char MotorIsSwitchOFF(MOTOR_STRUCT *motor);
char MotorIsAutoPowerOFF(MOTOR_STRUCT *motor);
char MotorIsPositionRel(MOTOR_STRUCT *motor);
char MotorIsFault(MOTOR_STRUCT *motor);




#endif
