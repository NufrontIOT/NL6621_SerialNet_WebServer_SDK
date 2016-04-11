/*
 * =====================================================================================
 *
 *       Fidatalename:  http_engine.c
 *
 *    Description:  Webserver engineer
 *
 *        Version:  0.0.1
 *        Created:  2015/6/12 17:27:25
 *       Revision:  none
 *
 *         Author:  Lin Hui (Link), linhui.568@163.com
 *   Organization:  Nufront
 *
 *--------------------------------------------------------------------------------------          
 *       ChangLog:
 *        version    Author      Date          Purpose
 *        0.0.1      Lin Hui    2015/6/12
 *        0.0.2		 Lin Hui    2015/9/25      增加系统状态显示页面      
 *
 * =====================================================================================
 */
#include "includes.h"
#include "lwip/mem.h"
#include "lwIP.h"
#include "lwIP/tcp.h"
#include "lwIP/udp.h"
#include "lwIP/tcpip.h"
#include "netif/etharp.h"
#include "lwIP/dhcp.h"
#include "ethernetif.h"
#include "arch/sys_arch.h"
#include "lwIP/sockets.h"
#include "lwIP/netdb.h"
#include "lwIP/dns.h"

#include "common.h"
#include "webserver.h"
#include "http_engine.h"
#include "http_parser.h"
#include "http_builder.h"

#include "wifi_opt.h"

HTTP_Table http_table[TCP_ENTRIES];
UINT8 server_stat = 0, client_stat = 0;

UINT8 SendData[SEND_BUFFER_SIZE];
extern struct netif netif;
extern int http_sock_fd;

void tcp_send(unsigned int idx, unsigned int len)
{
    if(idx < TCP_ENTRIES)
    {
        if (send(http_table[idx].sock_fd, SendData, len, 0) == -1)
            log_info("send error!\n");
    }

    return;
}

unsigned int http_sendfile(unsigned int idx, const char *name, unsigned char *tx)
{
    unsigned int len=0, i = 0;

    if (name) //start transfer -> add http header
    {
        http_table[idx].status = HTTP_SEND;
        http_table[idx].file   = http_fid(name, &http_table[idx].fparam);
        http_table[idx].ftype  = http_ftype(http_table[idx].file);
		http_table[idx].flen   = http_flen(http_table[idx].file, http_table[idx].fparam);
        http_table[idx].fpos   = 0;
        http_table[idx].fparse = 0;

        if(http_table[idx].flen == 0) //file not found
        {
            http_table[idx].status = HTTP_CLOSED;
            len = sprintf((char*)tx, HTTP_404_HEADER"Error 404 Not found\r\n\r\n");
            tcp_send(idx, len);
			log_info("Parse error: 404 Not found.\n");
            return len;
        }
        else //file found -> send http header
        {
            switch(http_table[idx].ftype)
            {
                case HTML_FILE:
                    len = sprintf((char*)tx, HTTP_HTML_HEADER"%i\r\n\r\n", http_table[idx].flen);
                    break;
                case JS_FILE:
                    len = sprintf((char*)tx, HTTP_JS_HEADER"%i\r\n\r\n", http_table[idx].flen);
                    break;
                case CSS_FILE:
                    len = sprintf((char*)tx, HTTP_CSS_HEADER"%i\r\n\r\n", http_table[idx].flen);
                    break;
                case ICON_FILE:
                    len = sprintf((char*)tx, HTTP_ICON_HEADER"%i\r\n\r\n", http_table[idx].flen);
                    break;
           }
           tx += len; 
        }
    }

	log_info("file:%d; ftype:%d; flen:%d\n", 
			http_table[idx].file,
			http_table[idx].ftype,
			http_table[idx].flen);

    if(http_table[idx].flen) //file found
    {
        switch(http_table[idx].ftype)
        {
            //dynamic content
            case HTML_FILE:
            case XML_FILE:
                do {
                    i = http_fparse((char*)tx, http_table[idx].file, &http_table[idx].fparse, 4096, http_table[idx].fparam); 
				   
				   	tx -= len; //重新设置HTTP头，值
				    len = sprintf((char*)tx, HTTP_HTML_HEADER"%i\r\n\r\n", i);
					tx[len] = '<';
					//memcpy(tx,(unsigned char *)rx_buffer,i);                 
				   
				    http_table[idx].fpos += i;
                    len += i;
					log_info("parse len:%d; fpos:%d; fparse:%d; len:%d\n", 
							i, http_table[idx].fpos, http_table[idx].fparse, len);
                    if (len <= 0)
                        break;
                    else
				    {
						//log_info("len:%d == SendData:%d=============\r\n\r\n%s\r\n\r\n=============\r\n\r\n",len,strlen((const char *)SendData),SendData);
                        tcp_send(idx, len);
				     }
                    tx = SendData;
					len = 0;
					log_info("parse len:%d; fpos:%d; fparse:%d; len:%d\n", 
							i, http_table[idx].fpos, http_table[idx].fparse, len);
                } while((http_table[idx].fpos < http_table[idx].flen));
                break;
            case JS_FILE:
            case CSS_FILE:
            case TXT_FILE:
            case ICON_FILE:
            case GIF_FILE:
            case JPEG_FILE:
                do {
                    i = http_fdata(tx, http_table[idx].file, http_table[idx].fpos, 4096); 
                    http_table[idx].fpos += i;
                    len += i;
                    if(len <= 0)
                        break;
                    else
                        tcp_send(idx, len);	
                    tx = SendData;
					len = 0;
                }while((http_table[idx].fpos < http_table[idx].flen));		
                break;
        }
		log_info("after send data: fpos:%d, len:%d\n", http_table[idx].fpos, len);
        if((http_table[idx].fpos >= http_table[idx].flen) || (len == 0))
        {
            http_close(idx);
            log_info("send file OK, close http!\n");
        }
    }

    return len;
}

void http_close(unsigned int idx)
{
    http_table[idx].status = HTTP_CLOSED;
    http_table[idx].flen   = 0;

    return;
}


void http_engine(unsigned int idx, char *rx, unsigned int rx_len, unsigned char *tx)
{	
    unsigned int tx_len = 0;
	static unsigned int updFlag=0;
	  
    switch (http_table[idx].status)
    {
        case HTTP_CLOSED:
            if (rx_len)
            {			    
                if(strncmpi(rx, "GET", 3) == 0)/*METHOD/part-to-resource-HTTP/Version-number*/
                {
					updFlag=0;
					log_info("GET Table form data.\n");
                    rx     += 3+1;
                    rx_len -= 3+1;
					if (strncmpi(rx, "/login.html", strlen("/login.html")) == 0)
					{
						log_info("Login system.\n");
						http_sendfile(idx, "/login.html", tx);
					} 
					else if (strncmpi(rx, "/status.html", strlen("/status.html")) == 0) 
					{
					 	log_info("Get system status html.\n");
						http_sendfile(idx, "/status.html", tx);
					}
					else if (strncmpi(rx, "/wifibase.html", strlen("/wifibase.html")) == 0) 
					{
					 	log_info("Get wifibase html.\n");
						http_sendfile(idx, "/wifibase.html", tx);
					}
					else if (strncmpi(rx, "/wifinet.html", strlen("/wifinet.html")) == 0) 
					{
					 	log_info("Get wifi network html.\n");
						http_sendfile(idx, "/wifinet.html", tx);
					}
					else if (strncmpi(rx, "/public.css", strlen("/public.css")) == 0) 
					{
					 	log_info("Get public css.\n");
						http_sendfile(idx, "/public.css", tx);
					}
					else 
					{
						log_info("Default login webserver.\n\n");
                    	http_sendfile(idx, "/login.html", tx);
					}
                }
                else if(strncmpi(rx, "POST", 4) == 0)
                {
					log_info("POST Table form data.\n");
					updFlag=0;
                    rx     += 4+1;
                    rx_len -= 4+1;
                    if (strncmpi(rx, "/reboot", 7) == 0)		/* process setupwifi form */
                    {
                        tx_len = sprintf((char*)tx, HTTP_200_HEADER);
						http_resetsys(idx, (char*)tx, tx_len);												         
                    }
				    else if (strncmpi(rx, "/factory", 8) == 0)  /* process resetwifi form */
					{
                        tx_len = sprintf((char*)tx, HTTP_200_HEADER);
						http_factory(idx, (char*)tx, tx_len);						
					}
					else if (strncmpi(rx, "/update", 7) == 0)  /* process resetwifi form */
					{	  
						updFlag=1;  
					}
				    else if (strncmpi(rx, "/wifibase", 9) == 0)  /* process resetwifi form */
					{
						http_setup_wifibase(idx, (char*)rx, rx_len, (char *)tx);
					}
				    else if (strncmpi(rx, "/setupuart", 10) == 0)  /* process resetwifi form */
					{
						http_setup_uart(idx, (char*)rx, rx_len, (char *)tx);
					}
				    else if (strncmpi(rx, "/setupnet", 9) == 0)  /* process resetwifi form */
					{
						http_setup_net(idx, (char*)rx, rx_len, (char *)tx);
					}
                    else
                    {
                        http_sendfile(idx, rx, tx);
                    }
                }
				else if(updFlag != 0)
				{					
					if(http_update(idx, (char*)rx, rx_len, (char *)tx, updFlag) == 0)
					{
						updFlag=0;
						printf("rebooting....\r\n");
						BSP_ChipReset();
					}
					else
						updFlag++;		
				}
                else
                {
                    tx_len = sprintf((char*)tx, HTTP_400_HEADER"Error 400 Bad request\r\n\r\n");
                    tcp_send(idx, tx_len);
                }
            }
            break;

        case HTTP_SEND:
            http_sendfile(idx, 0, tx);
            break;

        default:                                                      
            break;                                                          
    }                                                                         

    return;
}