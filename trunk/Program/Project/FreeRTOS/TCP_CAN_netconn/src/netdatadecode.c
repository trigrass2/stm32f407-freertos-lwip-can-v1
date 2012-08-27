#include "netdatadecode.h"
#include "globalstruct.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"


/*
********************���������������������ͷ����\0��β
*/
static int NetCommandSentenceDecode(struct netconn *sock,char* data, char** comm,uint16_t* pnum,char* pname[],char* pvalue[])
{
    char* cur_d=data;
	char* next_d;
	char* temp_d;
	if(NULL==data)
	{
		printf("the input data is nonvalidata!\r\n");
		return -1;//no validate data
	}
	temp_d=strchr(cur_d,'{');                                    //��������
	next_d=temp_d+1;                                             //ָ���һ����������
	if(NULL==temp_d)
	{
		printf("the input data has no {\r\n");
		return -1;//error format
	}
	temp_d--;
	while(*(temp_d)==' ')//jump to next command
	{
	   temp_d--;
	}
	temp_d++;
	*temp_d='\0';	                                             // �ѿո��Ϊ \0
	*comm=cur_d;//the command
	*pnum=0;
	cur_d=next_d;
	while(*cur_d!='\0')
	{
		temp_d=strchr(cur_d,' ');	                             //�Ҳ��� �������ڵĿո�
        if(NULL==temp_d)
        {
           printf("can't find ' ' for parameter name!\r\n");
           break;
        }
        *temp_d='\0';
        pname[*pnum]=cur_d;
        cur_d=temp_d+1;
       	while(*(cur_d)==' '&&*(cur_d)!='\0')
		{
			cur_d++;
		}
		temp_d=strpbrk(cur_d," }");                              //�����ھ������}��β
        if(NULL==temp_d)
        {
           printf("can't find '}' for parameter name!\r\n");
           break;
        }
        *temp_d='\0';                                            // }��Ϊ\0
        pvalue[*pnum]=cur_d;
        (*pnum)++;
        cur_d=temp_d+1;
        temp_d=strchr(cur_d,'{');                                //����������{ ��˵����������������������Ѱ��
        if(NULL==temp_d)
        {
           break;
        }
        cur_d=temp_d+1;
	}


return 0;//return ok;
}

/*the coommand link defined*/
typedef  struct _s_command_node
{
    NET_COMMAND_FORMATE_STRUCT const * comm;
    struct _s_command_node *next;
}s_command_node;

/*
		
*/
int NetDataDecode(struct netconn *sock,char* data)
{
	char * cur_d=data;
	char * next_d;
	char * temp_d;
	if(NULL==data)
	{
		printf("the input data is nonvalidata!\r\n");
		return -1;                                                      //no validate data
	}
	temp_d=strpbrk(data,"{}");                                          //Ѱ��data�е�һ��{
	if(NULL==temp_d)
	{
		printf("the input data has no '{' '}'\r\n");
		return -1;                                                      //error format
	}
	//package head corrent process
	//������������������ţ��������ͷ��������������������Ч������Ѱ����һ��\n
	if(*temp_d=='}'||*data=='{')                                        //the head is not break
	{
		temp_d=strchr(data,'\n');	                                    //�����ַ������״γ���\n��λ��
		if(NULL==temp_d)
		{
			//���ַ�����û��\n�򷵻أ�˵����������Ч
			printf("the input data has no '\n'");
			return -1;              //error format
		}
		else
		{
		    temp_d++;
			while(*(temp_d)==' '&&*(temp_d)!='\0')//jump to next command
			{
				temp_d++;
			}
		}
		cur_d=temp_d;//recorrect the current data head
	}
	//data decode
	while(*cur_d!='\0')
	{
		temp_d=strchr(cur_d,'\n');
		if(NULL==temp_d)
		{
			printf("the input data has no '\n' to the command end\r\n");
			return -1;//error format
		}
		*(temp_d)='\0';                                      //��\n ����\0
		temp_d++;
		while(*(temp_d)==' '&&*temp_d!='\0')//jump the space
		{
			//�ҵ���һ�����ͷ
			temp_d++;
		}
		next_d=temp_d;
		temp_d=strpbrk(cur_d,"{ }");//search to the '{', 'space byte' and '}'
		if(NULL!=temp_d)
		{
			uint16_t i,j;
			uint32_t command_list_num;
			char* comm=NULL;
			char* pname[NET_COMMAND_PARAMETER_MAX_NUM];
			char* pvalue[NET_COMMAND_PARAMETER_MAX_NUM];	
			int err;
			NET_COMMAND_FORMATE_STRUCT const * comm_format;
			uint16_t pnum=0;
			int comm_node[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};                            //Ĭ�������5��������ͬ
			uint16_t        comm_num=0;
			uint16_t        temp_comm_num=0;
			
			command_list_num=NetCommandNumber();                                  //�����������
			err=NetCommandSentenceDecode(sock,cur_d,&comm,&pnum,pname,pvalue);
			if(err)
			{
				printf("command formate is not correct\r\n");
				return err;
			}
			for(j=0;j<command_list_num;j++)
			{
				//�ҵ���Ӧ�����Ӧ��������кü�������ͬ������
				if((0==strcmp(sNetCommandFormat[j].comm,comm))&&(pnum==sNetCommandFormat[j].pnum))
				{
					comm_node[comm_num] = j;
					comm_num++;
				}
			}
			if(0==comm_num)
			{
				//δ�ҵ���ƥ�������
				printf("there no command correct \r\n");
				return -1;
			}
			temp_comm_num = comm_num;
			for(i=0;i<pnum;i++)//compare the paramter name to command formate list�Ƚϲ�������
			{				
				for(j=0;j<temp_comm_num;j++)
				{
					if(comm_node[j] == -1)   //�ýڵ��Ѳ�����Ҫ������
						continue;
	            	comm_format=&sNetCommandFormat[comm_node[j]];
	                if(0!=strcmp(comm_format->pname[i],pname[i])||(NULL!=comm_format->pvalue[i]&&0!=strcmp(pvalue[i],comm_format->pvalue[i])))
	                {
                        comm_node[j] = -1;
	                    if(comm_num>0)
						{
	                        comm_num--;
						}
						else
						{
	                        return -1;//return error
						}
	
					}
				}
			}
			if(1==comm_num)//find the command in format command list
			{
				for(i=0;i<temp_comm_num;i++)
				{
					if(-1!=comm_node[i])
					{
						comm_format=&sNetCommandFormat[comm_node[i]];
						break;
					}
				}
				if(NULL!=comm_format->callback)
				{
					(*comm_format->callback)(sock,pvalue,comm_node[i]);//call the value decode callback function
				}
				for(i=0;i<pnum;i++)
				{
					LCD_DisplayStringLine(LINE(5+i*2),   (uint8_t *)pname[i]);
					LCD_DisplayStringLine(LINE(5+i*2+1), (uint8_t *)pvalue[i]);
				}
			}
			else//fail to find the valid command
			{
				printf("error, the right command format is: %d\r\n",comm_num);

				return -1;//return error
			}
		}
		cur_d=next_d;
	}
	return 0;//return ok;
}
/*
*************ע���������**********************
************* REG {Type WR/R} {Period n} \r\n\0
*/
int REG_CommandDecode(struct netconn *sock,char* pvalue[])
{	
	char mode;
	int period;
	int err;
	int state;
	NET_CLIENT_USER_INFO_STRUCT *userinfo;
	//int idle_id;
	NET_COMMAND_FORMATE_STRUCT const *command_pointer = &sNetCommandFormat[0];
	
	
		//compare the preset mode and add net user
	if(0==strcmp(pvalue[0],"WR"))//read write mode
	{
		mode=NET_CLIENT_USER_MODE_WR;
	}
	else
	{
		if(0==strcmp(pvalue[0],"RO"))//read only mode
		{
			mode=NET_CLIENT_USER_MODE_RO;
		}
		else
		{
			printf("register command error\r\n");
			state=NET_COMMAND_RESPONSE_FAILED;
			err=-1;//fail return
			(*command_pointer->response)(sock,state);
			return err;
		}
	}
		//decode frequency value
		period=atoi(pvalue[1]);//string to int convert
		userinfo = NetUserSearchBySocket(sock);
		if(NULL != userinfo)
		{
			userinfo->mode=mode;
			userinfo->period=(int)period/100;
			state=NET_COMMAND_RESPONSE_OK;
			err=0;
		}
		else
		{
			state=NET_COMMAND_RESPONSE_INVALID;
		}
		(*command_pointer->response)(sock,state);//response for register command
		return err;//return successful
}

/*
******************                   �Ա���ֱ���ٶȿ���������н���          ****************************
************** DRIVE {Type Direct} {Name Velocity} {Left int} {Right int} {Normalized 1}\r\n\0*********
*/
int DRIVE_DirectVelocityDecode(struct netconn *sock,char* pvalue[],int command_number)
{
     NET_CLIENT_USER_INFO_STRUCT * userinfo=NetUserSearchBySocket(sock);
     NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT msg;
     NET_COMMAND_FORMATE_STRUCT const *command_pointer=&sNetCommandFormat[command_number];
     //int count;
     if(NULL==userinfo)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_INVALID);
		return -1;//no such user
	 }
	 if(userinfo->mode!=NET_CLIENT_USER_MODE_WR)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_READONLY);
		return -1;//the user have not right write
	 }
        memset((void*)(&msg),0,sizeof(NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT));
        msg.user_id=userinfo->user_id;
        msg.DATA.type=NET_ROBOT_DRIVER_TYPE_DIRECT;
        msg.DATA.mode=NET_ROBOT_DRIVER_MODE_VELOCITY;
	    msg.DATA.left_v  = atoi(pvalue[2]);
        msg.DATA.right_v = atoi(pvalue[3]);
        msg.DATA.norm=(char)atoi(pvalue[4]);
        msg.DATA.push=1;
	    msg.command_nubmer=command_number;
        //printf("command number is: %d\r\n",command_number);
        //count=������Ϣ������
        //if(count!=sizeof(NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT))
        if((NULL==xQ_DRIVE_COMM) || (xQueueSend( xQ_DRIVE_COMM, ( void * ) &msg, ( portTickType ) 0 ) != pdPASS))
		{
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_FAILED);
			printf("body queue fulled\r\n");
			return -1;
		}
		else
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_OK);
	    return 0;//return successful
}

/*
******************                   �Ա���ֱ�ӵ�������������н���          ****************************
************** DRIVE {Type Direct} {Name Velocity} {Left int} {Right int} {Normalized 1}\r\n\0*********
*/
int DRIVE_DirectCurrentDecode(struct netconn *sock,char* pvalue[],int command_number)
{
     NET_CLIENT_USER_INFO_STRUCT * userinfo=NetUserSearchBySocket(sock);
     NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT msg;
     NET_COMMAND_FORMATE_STRUCT const *command_pointer=&sNetCommandFormat[command_number];
     //int count;
     if(NULL==userinfo)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_INVALID);
		return -1;//no such user
	 }
	 if(userinfo->mode!=NET_CLIENT_USER_MODE_WR)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_READONLY);
		return -1;//the user have not right write
	 }
        msg.DATA.type=NET_ROBOT_DRIVER_TYPE_DIRECT;
        msg.user_id=userinfo->user_id;
        msg.DATA.type=NET_ROBOT_DRIVER_TYPE_DIRECT;
        msg.DATA.mode=NET_ROBOT_DRIVER_MODE_CURRENT;
		msg.DATA.left_c  = atoi(pvalue[2]);
        msg.DATA.right_c = atoi(pvalue[3]);
        msg.DATA.norm=(char)atoi(pvalue[4]);
        msg.DATA.push=1;
		msg.command_nubmer = command_number;
        if((NULL==xQ_DRIVE_COMM) || (xQueueSend(xQ_DRIVE_COMM, (void *)&msg, (portTickType)0 ) != pdPASS))
		{
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_FAILED);
			printf("body queue is fulled\r\n");
			return -1;
		}
		else
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_OK);
        return 0;//return successful
}
/*
******************                   �Ա����ӿ��ƿ���������н���  ��ת  ****************************
************** DRIVE {Type Indirect} {Name Velocity} {YawRate int} {Normalized 1}\r\n\0*********
*/
int DRIVE_IndirectDecode(struct netconn *sock,char* pvalue[],int command_number)
{
     NET_CLIENT_USER_INFO_STRUCT * userinfo=NetUserSearchBySocket(sock);
     NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT msg;
     NET_COMMAND_FORMATE_STRUCT const *command_pointer=&sNetCommandFormat[command_number];
     //int count;
     if(NULL==userinfo)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_INVALID);
		return -1;//no such user
	 }
	 if(userinfo->mode!=NET_CLIENT_USER_MODE_WR)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_READONLY);
		return -1;//the user have not right write
	 }
        memset((void*)(&msg),0,sizeof(NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT));
        msg.user_id=userinfo->user_id;
        msg.DATA.type=NET_ROBOT_DRIVER_TYPE_INDIRECT;
        msg.DATA.mode=NET_ROBOT_DRIVER_MODE_VELOCITY;
		msg.DATA.velocity  = atoi(pvalue[1]);
        msg.DATA.yawrate   = atoi(pvalue[2]);
        msg.DATA.norm=(char)atoi(pvalue[3]);
        msg.DATA.push=1;
		msg.command_nubmer=command_number;
        if((NULL==xQ_DRIVE_COMM) || (xQueueSend(xQ_DRIVE_COMM, (void *)&msg, (portTickType)0 ) != pdPASS))
		{
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_FAILED);
			printf("body queue is fulled\r\n");
			return -1;
		}
		else
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_OK);
        return 0;//return successful
}
/*
************   ��һ���ľ���������룬����mm *************************************************
************  GO {Type Ahead} {Distance int} {Velocity int} {Normalized 1} {Push 1} **********
*/
int GO_AheadDecode(struct netconn *sock,char* pvalue[],int command_number)
{
     NET_CLIENT_USER_INFO_STRUCT * userinfo=NetUserSearchBySocket(sock);
     NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT msg;
     NET_COMMAND_FORMATE_STRUCT const *command_pointer=&sNetCommandFormat[command_number];
     //int count;
     if(NULL==userinfo)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_INVALID);
		return -1;//no such user
	 }
	 if(userinfo->mode!=NET_CLIENT_USER_MODE_WR)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_READONLY);
		return -1;//the user have not right write
	 }
        memset((void*)(&msg),0,sizeof(NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT));
        msg.user_id=userinfo->user_id;
        msg.DATA.type=NET_ROBOT_DRIVER_TYPE_GOAHEAD;
        msg.DATA.mode=NET_ROBOT_DRIVER_MODE_POSITION;
		msg.DATA.go_distance  = atoi(pvalue[1]);
        msg.DATA.velocity   = atoi(pvalue[2]);
        msg.DATA.norm=(char)atoi(pvalue[3]);
        msg.DATA.push=(char)atoi(pvalue[4]);
		msg.command_nubmer=command_number;
        if((NULL==xQ_DRIVE_COMM) || (xQueueSend(xQ_DRIVE_COMM, (void *)&msg, (portTickType)0 ) != pdPASS))
		{
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_FAILED);
			printf("body queue is fulled\r\n");
			return -1;
		}
		else
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_OK);
        return 0;//return successful
}
/*
************   ��תһ���ĽǶȽ��룬�Ƕȵ�λ 0.1��     *************************************************
************  GO {Type Rotate} {Angle int} {YawRate int} {Normalized 1} {Push 1} **********
*/
int GO_RotateDecode(struct netconn *sock,char* pvalue[],int command_number)
{
     NET_CLIENT_USER_INFO_STRUCT * userinfo=NetUserSearchBySocket(sock);
     NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT msg;
     NET_COMMAND_FORMATE_STRUCT const *command_pointer=&sNetCommandFormat[command_number];
     //int count;
     if(NULL==userinfo)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_INVALID);
		return -1;//no such user
	 }
	 if(userinfo->mode!=NET_CLIENT_USER_MODE_WR)
	 {
		(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_READONLY);
		return -1;//the user have not right write
	 }
        memset((void*)(&msg),0,sizeof(NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT));
        msg.user_id=userinfo->user_id;
        msg.DATA.type=NET_ROBOT_DRIVER_TYPE_GOROTATE;
        msg.DATA.mode=NET_ROBOT_DRIVER_MODE_POSITION;
		msg.DATA.go_degree  = atoi(pvalue[1]);
        msg.DATA.yawrate    = atoi(pvalue[2]);
        msg.DATA.norm       = (char)atoi(pvalue[3]);
        msg.DATA.push       = (char)atoi(pvalue[4]);
		msg.command_nubmer=command_number;
        //count=write(userinfo->body_driver_pipe,(void*)&msg,sizeof(NET_ROBOT_CONTROL_DRIVER_MESSAGE_STRUCT));
        if((NULL==xQ_DRIVE_COMM) || (xQueueSend(xQ_DRIVE_COMM, (void *)&msg, (portTickType)0 ) != pdPASS))
		{
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_FAILED);
			printf("body queue is fulled\r\n");
			return -1;
		}
		else
			(*command_pointer->response)(sock,NET_COMMAND_RESPONSE_OK);
        return 0;//return successful
}
/*
***********************ͨ�÷�������
*/
int comm_CommandResponse(struct netconn *sock,int state)
{
	int8_t err;
    char data[100]="command state is: "; 
    strcat(data,gNetCommandResStr[state]);
	strcat(data,"\r\n");
    err = netconn_write(sock,data,strlen(data),NETCONN_COPY);
    if (err!=0)
		printf("Error, REG response send() failed with error code \r\n");
    return 0;
}

