/*
 * =====================================================================================
 *
 *       Filename:  webserver.c
 *
 *    Description:  Entry of webserver
 *
 *        Version:  0.0.1
 *        Created:  2015/6/12 15:47:43
 *       Revision:  none
 *
 *         Author:  Lin Hui (Link), linhui.568@163.com
 *   Organization:  Nufront
 *
 *--------------------------------------------------------------------------------------          
 *       ChangLog:
 *        version    Author      Date        Purpose
 *        0.0.1      Lin Hui    2015/6/12    Create and initialize
 *        0.0.2      Lin Hui    2015/9/18    Modify webserver framework to fix serialnet
 *
 * =====================================================================================
 */

#include "includes.h"

#include "common.h"
#include "webserver.h"
#include "http_engine.h"
#include "http_parser.h"


int conn_amount;
int http_sock_fd;

extern HTTP_Table http_table[TCP_ENTRIES];
extern UINT8 SendData[SEND_BUFFER_SIZE];
char *Rec_Data;

void webserver_main(void)
{
    int sock_fd, new_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    int ret;
    int i;

    fd_set fdsr;
    int maxsock;
 	struct timeval 	tvRead;

    log_info("Create web server task success.\n");

WebServer:
    conn_amount = 0;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        log_err("Create listening socket error!");
        return ;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(WEB_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        log_err("Webserver bind error!");
        return ;
    }

    if (listen(sock_fd, TCP_ENTRIES) == -1)
    {
        log_err("Webserver listen error!");
        return ;
    }

    log_info("Webserver listen port %d\n", WEB_PORT);
    maxsock = sock_fd ;
    sin_size = sizeof(client_addr);

    Rec_Data = OSMMalloc(RECV_BUFFER_SIZE);
    if (Rec_Data == NULL)
    {
        log_err("No memory\n");
        return ;
    }
    log_info("Rec_Data:%p &Rec_Data:%p \n",Rec_Data,&Rec_Data);
    for (i = 0; i < TCP_ENTRIES; i++)
    {
        http_table[i].status = HTTP_CLOSED;
        http_table[i].sock_fd = -1;
    }

    while (1)
    {
		FD_ZERO(&fdsr);
        FD_SET(sock_fd, &fdsr);

        for (i = 0; i < TCP_ENTRIES; i++)
        {
            if (http_table[i].sock_fd >= 0 && http_table[i].sock_fd < MEMP_NUM_NETCONN)
            {
                FD_SET(http_table[i].sock_fd, &fdsr);
            }
        }
	    tvRead.tv_sec =  2;
		tvRead.tv_usec = 0;
    	ret = select(maxsock + 1, &fdsr, NULL, NULL, &tvRead);
        if (ret <= 0)
        {
            log_info("Select timeout or error!");
			//检测是否为SofpAP
			if(SysParam.WiFiCfg.Protocol != PARAM_NET_MODE_SOFTAP)
			{
		        for (i = 0; i < TCP_ENTRIES; i++)
		        {
		            if (http_table[i].sock_fd >= 0)
		            {
					    http_table[i].status = HTTP_CLOSED;
		                close(http_table[i].sock_fd);
		                FD_CLR(http_table[i].sock_fd, &fdsr);
		                http_table[i].sock_fd = -1;
		                conn_amount--;	
		            }
		        }
				if(sock_fd >= 0)
				    close(sock_fd); 
				sock_fd = -1; 	
			    while(SysParam.WiFiCfg.Protocol != PARAM_NET_MODE_SOFTAP)
		   	    OSTimeDly(100); 
				goto WebServer;
			}
            continue;
        }

        for (i = 0; i < conn_amount; i++)
        {
            if (FD_ISSET(http_table[i].sock_fd, &fdsr))
            {
                ret = recv(http_table[i].sock_fd, Rec_Data, RECV_BUFFER_SIZE, 0);
                if (ret <= 0)
                {    
                    log_info("socket[%d] recv status[%d]\n", i,ret);
					http_table[i].status = HTTP_CLOSED;
					if(http_table[i].sock_fd >= 0)
				    {
	                    close(http_table[i].sock_fd);
	                    FD_CLR(http_table[i].sock_fd, &fdsr);
	                    http_table[i].sock_fd = -1;
	                    conn_amount--;
					}
                    for (i = 0; i < TCP_ENTRIES; i++)
                    {
                        if (http_table[i].sock_fd >= 0 && http_table[i].status == HTTP_CLOSED)
                        {
                            close(http_table[i].sock_fd);
                            FD_CLR(http_table[i].sock_fd, &fdsr);
                            http_table[i].sock_fd = -1;
                            conn_amount--;	
                        }
                    }
                    log_err("conn_amount=%d\n",conn_amount);
                }
                else
                {  
                    if (ret < RECV_BUFFER_SIZE)
                        Rec_Data[ret] = '\0';
                    memset(SendData, 0, SEND_BUFFER_SIZE);

					/* Debug http data */
					//log_info("Get http client data:\n%s\n\n", Rec_Data);

                    http_engine(i, Rec_Data, ret, SendData);
                }
            }
        }

        if (FD_ISSET(sock_fd, &fdsr))
        {
            if (conn_amount >= TCP_ENTRIES)
            {
                for (i = 0; i < TCP_ENTRIES; i++)
                {
                    if (http_table[i].sock_fd >= 0 && http_table[i].status == HTTP_CLOSED)
                    {
                        close(http_table[i].sock_fd);
                        FD_CLR(http_table[i].sock_fd, &fdsr);
                        http_table[i].sock_fd = -1;
                        conn_amount--;	
                    }
                } 	
            }

            new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
            if (new_fd < 0)
            {
                log_info("accept socket error!");
                continue;
            }

            if (conn_amount < TCP_ENTRIES)
            {
                http_table[conn_amount++].sock_fd = new_fd;
                log_info("new connection client[%d] %s:%d\n", 
                        conn_amount,
						inet_ntoa(client_addr.sin_addr), 
                        ntohs(client_addr.sin_port));
                maxsock = new_fd + 1;
            }
            else
            {
                log_info("max connections arrive, exit\n");
                close(new_fd);
            }
        }		
	}
}


void webserver_init()
{
    http_senddata_alloc();
}														  

extern struct sys_status_t sys_status;

void WebServerThread(void * arg)
{

 	//只能在SOFTAP模式
    while(SysParam.WiFiCfg.Protocol != PARAM_NET_MODE_SOFTAP || link_status != SYS_EVT_LINK_UP)
   	     OSTimeDly(100); 

    webserver_init();

	while (1) {	  

	   	webserver_main();
	}
}


