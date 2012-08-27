

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
u32_t  e_time_ms = 0;                                                           //记录系统开机时间，100ms为单位

void Get_100ms_thread(void *arg);
/*
**************关闭并删除某连接**************
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
*****************反馈信息处理进程**********************
*****************获取反馈信息100ms处理一次*************
*****************通过信号量获取时间********************
*/
static void NetFeedBack_thread(void *arg)
{
	int i;
	vSemaphoreCreateBinary(xSemTime);                        //创建信号量
	if(xSemTime ==NULL)
	{
		printf("xSemTime creat error!\r\n");
	}
	xTaskCreate(Get_100ms_thread, "GET100MS", configMINIMAL_STACK_SIZE, NULL,TIMER_THEAD_PRIO, NULL);
	while(1)
	{
		if( xSemaphoreTake( xSemTime, ( portTickType ) 50 ) == pdTRUE )         //获取信号量,成功则说明时间到
		{
			//    数据处理函数
			sprintf(gNetBuffer,"time has eslaped: %.5d\r\n",e_time_ms);
			gNetDataSize = 26;
			for(i=0;i<MAX_CLIENT_NUM;i++)                                      //检查哪些有数据需要发送
			{
				  if(((0!=user_info[i].is_used)&&(0!=user_info[i].period)&&(0==e_time_ms%user_info[i].period))||(1==user_info[i].req))
				  {
				  		user_info[i].send_flag = 1;		           //数据发送标志位置1
						user_info[i].req=0;						   //请求标志位置0
				  }
			}
		}
		//vTaskDelay(100);
	}

}

/*
*****************精确定时线程，100ms产生一个定时*******
*/
static void Get_100ms_thread(void *arg)
{
	char disp[20];
	while(1)
	{
		vTaskDelay(100);                     //延时100ms
		e_time_ms ++;
		xSemaphoreGive(xSemTime);            //释放信号量
		sprintf(disp,"time ms: %.3d\0",e_time_ms);
		LCD_DisplayStringLine(Line6, (uint8_t*)disp);	
	}	
}
/*
********************处理具体链接的函数*******************************
**********tcp_netconn_thread接收到一个连接后创建此任务***************
***************     该任务可重入函数  *******************************
*/
static void NetUserProcess_thread(void *arg)
{
	NET_CLIENT_USER_INFO_STRUCT *this_info = (NET_CLIENT_USER_INFO_STRUCT *)arg;
	struct netconn *t_conn = this_info->conn;
	//int count = 0;
	u16_t buflen=15;
	struct netbuf *inbuf;
    char* buf;
	char  rsv_buf[100] = "wlcom to conect me^_^\r\n";                                   // 长度为100的接收缓冲区，存放接收到的数据
	t_conn->recv_timeout = 50;
	//this_info->period = 10;                             //设置超时时间，不阻塞接收
	netconn_write(t_conn,rsv_buf,strlen(rsv_buf),NETCONN_NOCOPY);
	while(1)
	{
	    if(t_conn->err == ERR_CLSD)                       //连接已被关闭，退出该线程
		{
			printf("a connect clsoed \r\n");
			goto exit;	
		}
		inbuf = netconn_recv(t_conn);                     //不阻塞接收数据
		//处理接收到的命令
		if (inbuf != NULL)
		{
			//LCD_DisplayStringLine(Line5, (uint8_t*)"get data!\0");
			netbuf_data(inbuf, (void**)&buf, &buflen);    //获取数据及长度
			if(buflen>100)
			{
				printf("receive data is too long \r\n");
				netbuf_delete(inbuf);
			}
			strncpy(rsv_buf,buf,buflen);                  //复制数据至 接收缓冲数组 rsv_buf
			netbuf_delete(inbuf);                         //释放内部分配的存储空间
			rsv_buf[buflen]='\0';                         //设置接收到的字符串结尾
			//该处调用命令处理函数对命令进行处理
			NetDataDecode(t_conn,rsv_buf);                //数据处理函数
		    //netconn_write(t_conn,rsv_buf,buflen,NETCONN_NOCOPY);
	        
		}
		//处理需要发送的数据
		if(this_info->send_flag!=0)                        //有数据要发送
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
  * TCP 连接线程，有它创建其他线程
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
  /*创建返回数据处理进程，该进程处理feedback数据*/
  xTaskCreate(NetFeedBack_thread, "FEEDBACKPROCESS", NET_FEEDBACK_THREAD_STACKSIZE, NULL,FEED_BACK_PRIO, NULL);
  conn = netconn_new(NETCONN_TCP);
  conn->recv_timeout = 10000;              //等待连接超时时间为10s

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
		if(NULL != newconn)					  //成功连接
		{
			printf("a client connected to the server ");
	      	if(client_used<MAX_CLIENT_NUM)    //连接数小于最大连接数
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
						/*设定新任务名字*/
						sprintf(thread_name,"USERPROCESS%d\0",thread_num);
						/*创建新任务*/
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
				//超出最大连接数据，关闭连接
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
  * 创建TCP服务器线程
  */
void tcp_netconn_init()
{
  sys_thread_new("TCP_CONNECT", tcp_netconn_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCP_THREAD_PRIO);
}
