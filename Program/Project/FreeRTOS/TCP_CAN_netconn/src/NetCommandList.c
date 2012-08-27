#include "NetCommandList.h"
#include "netdatadecode.h"
const NET_COMMAND_FORMATE_STRUCT sNetCommandFormat[]=
{
  /*command, pnum  ,parameter name,                             parameter value,command callback function pointer,  response callback function*/
  {"REG",     2,  {"Mode","Period"},                           {NULL},                  REG_CommandDecode,                         comm_CommandResponse},
  {"DRIVE",   5,  {"Type","Name","Left","Right","Normalized"}, {"Direct","Current"},    DRIVE_DirectCurrentDecode,                         comm_CommandResponse},
  {"DRIVE",   5,  {"Type","Name","Left","Right","Normalized"}, {"Direct","Velocity"},   DRIVE_DirectVelocityDecode,                         comm_CommandResponse},
  {"DRIVE",   4,  {"Type","Velocity","YawRate","Normalized"},         {"Indirect"},     DRIVE_IndirectDecode,                         comm_CommandResponse},
  {"GO",      5,  {"Type","Distance","Velocity","Normalized","Push"}, {"Ahead"},        GO_AheadDecode,                         comm_CommandResponse},
  {"GO",      5,  {"Type","Angle","YawRate","Normalized","Push"},     {"Rotate"},       GO_RotateDecode,                         comm_CommandResponse},
};

/*------------------------------------------------------------------------
*caculate the command number
* 获取命令个数
*/
int NetCommandNumber(void)	       
{
	return (sizeof(sNetCommandFormat)/sizeof(sNetCommandFormat[0]));
}
