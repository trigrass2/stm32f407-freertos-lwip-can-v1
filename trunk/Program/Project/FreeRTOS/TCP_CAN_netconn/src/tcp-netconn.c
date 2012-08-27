

/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "fs.h"
#include "string.h"
#include "tcp-netconn.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "globalstruct.h"
#include "main.h"
#include "netdatadecode.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

xSemaphoreHandle xSemTime;
struct netconn *n_conn[MAX_CLIENT_NUM];
NET_CLIENT_USER_INFO_STRUCT user_info[MAX_CLIENT_NUM] = {{0,0,0,0,0,0,NULL},{0,0,0,0,0,0,NULL},{0,0,0,0,0,0,NULL}};
int client_used=0;
u32_t  e_time_ms = 0;                                                           //��¼ϵͳ����ʱ�䣬100msΪ��λ

void Get_100ms_thread(void *arg);
/*
**************�رղ�ɾ��ĳ����**************
*/
void delete_conn_u(struct netconn *conn)
{
	 int i;
	 netconn_close(conn);
	 netconn_delete(conn);
	 for(i=0;i<MAX_CLIENT_NUM;i++)
	 {
	 	if(user_info[i].conn == conn)
		{	
			  user_info[i].conn = NULL;
			  user_info[i].is_used =0;
			  client_used--;
			  break;
		}
	 }
}



/*
*****************������Ϣ�������**********************
*****************��ȡ������Ϣ100ms����һ��*************
*****************ͨ���ź�����ȡʱ��********************
*/
static void NetFeedBack_thread(void *arg)
{
	int i;
	vSemaphoreCreateBinary(xSemTime);                        //�����ź���
	if(xSemTime ==NULL)
	{
		printf("xSemTime creat error!\r\n");
	}
	xTaskCreate(Get_100ms_thread, "GET100MS", configMINIMAL_STACK_SIZE, NULL,TIMER_THEAD_PRIO, NULL);
	while(1)
	{
		if( xSemaphoreTake( xSemTime, ( portTickType ) 50 ) == pdTRUE )         //��ȡ�ź���,�ɹ���˵��ʱ�䵽
		{
			//    ���ݴ�����
			sprintf(gNetBuffer,"time has eslaped: %.5d\r\n",e_time_ms);
			gNetDataSize = 26;
			for(i=0;i<MAX_CLIENT_NUM;i++)                                      //�����Щ��������Ҫ����
			{
				  if(((0!=user_info[i].is_used)&&(0!=user_info[i].period)&&(0==e_time_ms%user_info[i].period))||(1==user_info[i].req))
				  {
				  		user_info[i].send_flag = 1;		           //���ݷ��ͱ�־λ��1
						user_info[i].req=0;						   //�����־λ��0
				  }
			}
		}
		//vTaskDelay(100);
	}

}

/*
*****************��ȷ��ʱ�̣߳�100ms����һ����ʱ*******
*/
static void Get_100ms_thread(void *arg)
{
	char disp[20];
	while(1)
	{
		vTaskDelay(100);                     //��ʱ100ms
		e_time_ms ++;
		xSemaphoreGive(xSemTime);            //�ͷ��ź���
		sprintf(disp,"time ms: %.3d\0",e_time_ms);
		LCD_DisplayStringLine(Line6, (uint8_t*)disp);	
	}	
}
/*
********************����������ӵĺ���*******************************
**********tcp_netconn_thread���յ�һ�����Ӻ󴴽�������***************
***************     ����������뺯��  *******************************
*/
static void NetUserProcess_thread(void *arg)
{
	NET_CLIENT_USER_INFO_STRUCT *this_info = (NET_CLIENT_USER_INFO_STRUCT *)arg;
	struct netconn *t_conn = this_info->conn;
	//int count = 0;
	u16_t buflen=15;
	struct netbuf *inbuf;
    char* buf;
	char  rsv_buf[100] = "wlcom to conect me^_^\r\n";                                   // ����Ϊ100�Ľ��ջ���������Ž��յ�������
	t_conn->recv_timeout = 50;
	//this_info->period = 10;                             //���ó�ʱʱ�䣬����������
	netconn_write(t_conn,rsv_buf,strlen(rsv_buf),NETCONN_NOCOPY);
	while(1)
	{
	    if(t_conn->err == ERR_CLSD)                       //�����ѱ��رգ��˳����߳�
		{
			printf("a connect clsoed \r\n");
			goto exit;	
		}
		inbuf = netconn_recv(t_conn);                     //��������������
		//������յ�������
		if (inbuf != NULL)
		{
			//LCD_DisplayStringLine(Line5, (uint8_t*)"get data!\0");
			netbuf_data(inbuf, (void**)&buf, &buflen);    //��ȡ���ݼ�����
			if(buflen>100)
			{
				printf("receive data is too long \r\n");
				netbuf_delete(inbuf);
			}
			strncpy(rsv_buf,buf,buflen);                  //���������� ���ջ������� rsv_buf
			netbuf_delete(inbuf);                         //�ͷ��ڲ�����Ĵ洢�ռ�
			rsv_buf[buflen]='\0';                         //���ý��յ����ַ�����β
			//�ô��������������������д���
			NetDataDecode(t_conn,rsv_buf);                //���ݴ�����
		    //netconn_write(t_conn,rsv_buf,buflen,NETCONN_NOCOPY);
	        
		}
		//������Ҫ���͵�����
		if(this_info->send_flag!=0)                        //������Ҫ����
		{
			  netconn_write(t_conn,gNetBuffer,gNetDataSize,NETCONN_NOCOPY);
			  this_info->send_flag = 0;
		}
		else
		{
		    //nodata
		}
		//vTaskDelay(100);
	}
exit:
    delete_conn_u(t_conn);
	vTaskDelete( NULL );
	//return 0;
}

/**
  * @brief  TCP server thread 
  * @param arg: pointer on argument(not used here) 
  * @retval None
  * TCP �����̣߳��������������߳�
  */
static void tcp_netconn_thread(void *arg)
{ 
  struct netconn *conn, *newconn;
  err_t err;
  int i;
  //char disp[20]="get a connect!\0";
  char thread_name[16];
  int thread_num=0;
  /* Create a new TCP connection handle */
  /*�����������ݴ�����̣��ý��̴���feedback����*/
  xTaskCreate(NetFeedBack_thread, "FEEDBACKPROCESS", NET_FEEDBACK_THREAD_STACKSIZE, NULL,FEED_BACK_PRIO, NULL);
  conn = netconn_new(NETCONN_TCP);
  conn->recv_timeout = 10000;              //�ȴ����ӳ�ʱʱ��Ϊ10s

  if (conn!= NULL)
  {
    /* Bind to port 10000 with default IP address */
    err = netconn_bind(conn, NULL, 10000);
    
    if (err == ERR_OK)
    {
      /* Put the connection into LISTEN state */
      netconn_listen(conn);
  	  printf(" start to listen the connect \r\n");
      while(1) 
      {
        /* accept any icoming connection */
        newconn = netconn_accept(conn);
		if(NULL != newconn)					  //�ɹ�����
		{
			printf("a client connected to the server ");
	      	if(client_used<MAX_CLIENT_NUM)    //������С�����������
			{
				client_used++;
				for(i=0;i<MAX_CLIENT_NUM;i++)
				{
					if(user_info[i].conn==NULL)
					{
						user_info[i].conn =  newconn;
					    user_info[i].is_used = 1;
						user_info[i].user_id = ++thread_num;
						user_info[i].period = 0;
						/*�趨����������*/
						sprintf(thread_name,"USERPROCESS%d\0",thread_num);
						/*����������*/
						LCD_DisplayStringLine(Line3, (uint8_t*)thread_name);
						printf(thread_name);
						printf("  new client thread\r\n");
						//netconn_write(newconn,thread_name,13,NETCONN_NOCOPY);
						xTaskCreate(NetUserProcess_thread, thread_name, NET_USERPROCESS_STACKSIZE ,(void*)&user_info[i], USERPROCESS_THREAD_PRIO,NULL);
						break;
					}
				}	
			}
			else
			{
				//��������������ݣ��ر�����
				netconn_write(newconn,"net user is reach the max number\r\n\0",32,NETCONN_COPY);
				netconn_close(newconn);
				printf("net user is reach the max number,close it\r\n\0");
			}
		}
		else
		{
			  vTaskDelay(100);
		} 
        /* serve connection */
        //http_server_serve(newconn);
      
        /* delete connection */
        //netconn_delete(newconn);
      }
    }
    else
    {
      printf("can not bind netconn");
    }
  }
  else
  {
     printf("can not create netconn");
  }
}

/**
  * @brief  Initialize the HTTP server (start its thread) 
  * @param  none
  * @retval None  
  * ����TCP�������߳�
  */
void tcp_netconn_init()
{
  sys_thread_new("TCP_CONNECT", tcp_netconn_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCP_THREAD_PRIO);
}
