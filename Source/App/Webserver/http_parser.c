/*
 * =====================================================================================
 *
 *       Fidatalename:  http_parser.c
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
 *        0.0.2		 Lin Hui    2015/9/24      增加系统状态显示页面      
 *
 * =====================================================================================
 */
#include "includes.h"
#include "http_engine.h"
#include "http_parser.h"
#include "http_builder.h"

#include "wifi_opt.h"


#define HTTP_SEND_BUFFER_MAX		(4096)

int wl_workmode = 0;
extern USER_CFG_PARAM UserParam;
extern VOID SaveUserParam(VOID);
extern unsigned int http_sendfile(unsigned int idx, const char *name, unsigned char *tx);

/* malloc html send buffer 4096 Bytes */
unsigned char *html_buf;

unsigned int get_web_html_len(unsigned int type)
{
    FW_HDR fw_hdr;
	UINT32 Addr;

	switch (type) 
	{
		case 0:
			Addr = HTML_LOGIN_ADDR;
			break;
		case 1:
			Addr = HTML_LOGIN_ADDR;
			break;
		case 2:
			Addr = HTML_SYS_STATUS_ADDR;
			break;
		case 3:
			Addr = HTML_WIFI_SETUP_ADDR;
			break;
		case 4:
			Addr = HTML_WIFI_NET_ADDR;
			break;
		case 5:
			Addr = PUBLIC_CSS_FORMAT_ADDR;
			break;
		default:
			return 0;
	}

    QSpiFlashRead(Addr, &fw_hdr, sizeof(FW_HDR));

    return (fw_hdr.FwSize - 0x100 + 1);
}

unsigned char* get_web_html(unsigned int type)
{
	UINT32 Addr;
	unsigned int len;

	switch (type) 
	{
		case 0:
			Addr = HTML_LOGIN_ADDR;
			break;
		case 1:
			Addr = HTML_LOGIN_ADDR;
			break;
		case 2:
			Addr = HTML_SYS_STATUS_ADDR;
			break;
		case 3:
			Addr = HTML_WIFI_SETUP_ADDR;
			break;
		case 4:
			Addr = HTML_WIFI_NET_ADDR;
			break;
		case 5:
			Addr = PUBLIC_CSS_FORMAT_ADDR;
			break;
		default:
			return NULL;
	}
    len = get_web_html_len(type);

	memset(html_buf, 0x0, HTTP_SEND_BUFFER_MAX);
    QSpiFlashRead(Addr + FLASH_PAGE_SIZE, html_buf, len);

	log_info("Read html data from nofflash(addr:0x%x, len:%d)\n", Addr, len);

    return html_buf;
}


#define HTTPFILES (6)
const HTTPFILE httpfiles[HTTPFILES] = 
{
    {"/",            	HTML_FILE, 	get_web_html,	get_web_html_len},
    {"/login_0x6A000.html",     HTML_FILE, 	get_web_html, 	get_web_html_len},
    {"/status_0x6B000.html",   	HTML_FILE, 	get_web_html,  	get_web_html_len},
    {"/wifibase_0x6C000.html", 	HTML_FILE, 	get_web_html,  	get_web_html_len},
	{"/wifinet_0x6D000.html", 	HTML_FILE, 	get_web_html,  	get_web_html_len},
    {"/public_0x69000.css", 	CSS_FILE, 	get_web_html,  	get_web_html_len},
};

void http_senddata_alloc(void)
{
	/* malloc 4Kbytes size for http send buffer */
	html_buf = malloc(HTTP_SEND_BUFFER_MAX);
	if (html_buf == NULL)
    {
        log_err("No memory\n");
        return ;
    }
}


unsigned int http_fparse(char *dst, unsigned int file, unsigned int *start, unsigned int len, unsigned int param)
{
    unsigned int data_len = 0;
    char *src;
    char c;

    if(file >= HTTPFILES)
    {
        return 0;
    }

    src = (char*)httpfiles[file].data(file);
	if (src == NULL) return 0;

    if(len == 0) //calc file size
    {
        for(;;)
        {
            c = *src++;
            if(c == 0)
            {
                break;
            }
            data_len++;
        }
    }
	else if (file == 2) /*status.html web page, rebuild it */
	{
		data_len = html_rebuild_sysstatus((unsigned char*)dst, (unsigned char*)(src + *start), len);
	}
	else if (file == 3) /*wifibase.html web page, rebuild it */
	{
		data_len = html_rebuild_wifibase((unsigned char*)dst, (unsigned char*)(src + *start), len);
	}
	else if (file == 4) /*wifinet.html web page, rebuild it */
	{
		data_len = html_rebuild_wifinet((unsigned char*)dst, (unsigned char*)(src + *start), len);
	}
    else
    {
        src += *start;
        for(;;)
        {
            c = *src++;
            *start += 1;
            if(c == 0)
            {
                break;
            }
            *dst++ = c;
            data_len++;
            if(data_len >= len)
            {
                break;
            }
        }
    }

	log_info("Parse http data finish. data len:%d.\n", data_len);

    return data_len;
}


unsigned int http_fdata(unsigned char *dst, unsigned int file, unsigned int start, unsigned int len)
{
    if(file >= HTTPFILES)
    {
        return 0;
    }

    if((start+len) > httpfiles[file].len(file))
    {
        len = httpfiles[file].len(file) - start;
    }

    memcpy(dst, httpfiles[file].data(file) + start, len);
	log_info("Parse other type data file finish. data len:%d.\n", len);

    return len;
}


unsigned int http_flen(unsigned int file, unsigned int param)
{
    if(file >= HTTPFILES)
    {
        return 0;
    }

    switch(httpfiles[file].type)
    {
        case TXT_FILE:
        case HTML_FILE:
        case XML_FILE:
            return http_fparse(0, file, 0, 0, param);
    }

    return httpfiles[file].len(file);
}


unsigned int http_ftype(unsigned int file)
{
    if(file >= HTTPFILES)
    {
        return UNKNOWN_FILE;
    }

    return httpfiles[file].type;
}


unsigned int http_fid(const char *name, unsigned int *param)
{
    unsigned int file, len, i;
    char buf[8];

    if(name == 0)
    {
        return 0xFFFF;
    }

    *param = 0;

    while (*name == ' '){ name++; } //skip spaces

    for (file=0; file < HTTPFILES; file++)
    {
        len = strlen(httpfiles[file].name);
        if (strncmpi(name, httpfiles[file].name, len) == 0)
        {
            if ((name[len] == 0) || (name[len] == ' ')) //file found
            {
                break;
            }
            else if (name[len] == '?') ////file found, with parameter
            {
                name += len + 1; //skip name and "?"
                for (i=0; i<8;)
                {
                    if (isdigit(*name))
                    {
                        buf[i++] = *name++;
                    }
                    else
                    {
                        break;
                    }
                }
                buf[i] = 0;
                *param = atoi(buf);
                break;
            }
        }
    }

    if (file >= HTTPFILES)
    {
        return 0xFFFF;
    }

    return file;
}



unsigned int url_decode(char *dst, const char *src, unsigned int len)
{
    unsigned int i;
    char c, *ptr, buf[4]={0,0,0,0};

    ptr = dst; //save dst

    for (i = 0; i < len; )
    {
        c = *src++; i++;
        if((c == 0)    || 
                (c == '&')  ||
                (c == ' ')  ||
                (c == '\n') ||
                (c == '\r'))
        {
            break;
        }
        else if(c == '%')
        {
            buf[0] = *src++; i++;
            buf[1] = *src++; i++;
            *dst++ = (unsigned char)atou_hex(buf);
        }
        else if(c == '+')
        {
            *dst++ = ' ';
        }
        else
        {
            *dst++ = c;
        }
    }

    *dst = 0;

    //remove space at start and end of string
    strrmvspace(ptr, ptr);

    return i;
}


char* http_skiphd(const char *src, unsigned int *len)
{
    unsigned int i;

    for (i = *len; i != 0; i--, src++)
    {
        if(i >= 4)
        {
            if((src[0] == '\r') && (src[1] == '\n') && (src[2] == '\r') && (src[3] == '\n'))
            {
                src += 4;
                i   -= 4;
                break;
            }
        }
    }

    *len = i;

    return (char*)src;
}



int http_resetsys(unsigned int idx, char *tx, unsigned int tx_len)
{
	http_sendfile(idx, "/login_0x6A000.html", (unsigned char *)tx);
	OSTimeDly(10);
	UserParam.atMode = wl_workmode;  
    UserParam.isValid = USER_PARAM_FLAG_VALID; 
	SaveUserParam();
  	OSTimeDly(10);
	BSP_ChipReset();
	return 0;
}



int http_factory(unsigned int idx, char *tx, unsigned int tx_len)
{
	http_sendfile(idx, "/login_0x6A000.html", (unsigned char *)tx);
    memset((void*)&UserParam, 0, sizeof(USER_CFG_PARAM));
    SaveUserParam();

    OSTimeDly(10);
    BSP_ChipReset();
	return 0;
}


void http_setup_wifibase(unsigned int idx, char *rx, unsigned int rx_len, char *tx)
{
    unsigned int i = 0;
	unsigned int tx_len;
    char *ssid = 0, *key = 0;
	char *ipaddr = 0, *netmask = 0, *gateway = 0, *dnsserv = 0;
	char *protocol = 0, *authmode = 0, *encrymode = 0;
    char *channel = 0 , *dhcp = 0;
	char *web_ssid = 0, *web_key = 0, *web_ipaddr = 0, *web_netmask = 0;//web
	int wl_ipaddr, wl_netmask, wl_gateway, wl_dnsserv;
	int wl_protocol, wl_authmode, wl_encrymode, wl_channel,wl_dhcp;


	wl_protocol = 0;
	tx_len = tx_len;
	/* translate WIFI field */
    for (; rx_len!=0;)
    {
	    if(strncmpi(rx, "wifimode=", 9) == 0)
        { rx += 9; rx_len -= 9; protocol = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i;  wl_protocol = atoi(protocol);}      
		else if(strncmpi(rx, "encry=", 6) == 0)
        { rx += 6; rx_len -= 6; encrymode = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i;}	 
		else if(strncmpi(rx, "authmode=", 9) == 0)
        { rx += 9; rx_len -= 9; authmode = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i;}	 
		else if(strncmpi(rx, "apssid=", 7) == 0)
        { rx += 7; rx_len -= 7; if(wl_protocol == 2) ssid = rx; else web_ssid = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }	
        else if(strncmpi(rx, "apkey=", 6) == 0)
        { rx += 6; rx_len -= 6; if(wl_protocol == 2) key = rx; else web_key = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }	
        else if(strncmpi(rx, "apipaddr=", 9) == 0)
        { rx += 9; rx_len -= 9; if(wl_protocol == 2) ipaddr = rx; else  web_ipaddr = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }	
        else if(strncmpi(rx, "apnetmask=", 10) == 0)
        { rx += 10; rx_len -= 10;if(wl_protocol == 2) netmask = rx; else web_netmask = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
        else if(strncmpi(rx, "channel=", 8) == 0)
        { rx += 8; rx_len -= 8; channel = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
		else if(strncmpi(rx, "stassid=", 8) == 0)
        { rx += 8; rx_len -= 8; if(wl_protocol == 0) ssid = rx; else web_ssid = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }	
        else if(strncmpi(rx, "stakey=", 7) == 0)
        { rx += 7; rx_len -= 7; if(wl_protocol == 0) key = rx; else web_key = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }		
        else if(strncmpi(rx, "dhcp=", 5) == 0)
        { rx += 5; rx_len -= 5; dhcp = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }			
        else if(strncmpi(rx, "staipaddr=", 10) == 0)
        { rx += 10; rx_len -= 10; if(wl_protocol == 0) ipaddr = rx; else web_ipaddr = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }	
        else if(strncmpi(rx, "stanetmask=", 11) == 0)
        { rx += 11; rx_len -= 11; if(wl_protocol == 0) netmask = rx; else web_netmask = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
        else if(strncmpi(rx, "gateway=", 8) == 0)
        { rx += 8; rx_len -= 8; gateway = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
		else if(strncmpi(rx, "dnsserv=", 8) == 0)
        { rx += 8; rx_len -= 8; dnsserv = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
        else if(strncmpi(rx, "save=", 5) == 0)
        { rx += 5; rx_len -= 5; }
        else
        { rx++; rx_len--; }
    }

    inet_aton(web_ipaddr, &UserParam.web_IPCfg.Ip);				// ipv4 address
	inet_aton(web_netmask, &UserParam.web_IPCfg.Netmask);
  	//ssid
	NST_MOVE_MEM(UserParam.web_ssid,web_ssid,strlen(web_ssid));
   	UserParam.web_ssid[strlen(web_ssid)] = 0;
	//key
	NST_MOVE_MEM(UserParam.web_key,web_key,strlen(web_key));
	UserParam.web_key[strlen(web_key)] = 0;

	inet_aton(ipaddr, &wl_ipaddr);
	inet_aton(netmask, &wl_netmask);

    inet_aton(gateway, &wl_gateway);
	inet_aton(dnsserv, &wl_dnsserv);

    wl_authmode = atoi(authmode);
    wl_encrymode = atoi(encrymode);
	wl_channel = atoi(channel);
	wl_dhcp = atoi(dhcp);

	/* Check WIFI parameters valid */
	if (wl_protocol < 0 || wl_protocol > 2 ||
		wl_authmode < 0 || wl_authmode > 3 ||
		wl_encrymode < 0 || wl_encrymode > 4 ||
		wl_channel < 0 || wl_channel > 13) 
	{
		log_info("Form data(setupwifi) error.\n");
		return;	
	}
	log_info("ssid:%s, key:%s, protocol:%d, encry:%d, authmode:%d, channel:%d.\n",
				ssid, key, wl_protocol, wl_encrymode, wl_authmode, wl_channel);
	
	//DHCP
	if(wl_protocol == 0)
      	UserParam.cfg.IPCfg.bDhcp = wl_dhcp;
	else if(wl_protocol == 2)
		UserParam.cfg.bDhcpServer = 1;

	SetWifiParam(ssid, key, wl_ipaddr, wl_netmask, wl_gateway, wl_dnsserv,
			wl_protocol, wl_encrymode, wl_authmode, wl_channel);

 	http_sendfile(idx, "/wifibase_0X6C000.html", (unsigned char *)tx);
	 
}


void http_setup_uart(unsigned int idx, char *rx, unsigned int rx_len, char *tx)
{
    unsigned int i = 0;
	unsigned int tx_len;
	char *baudrate = 0, *framelen = 0, *frametime = 0;
	int wl_baudrate, wl_framelen, wl_frametime;

	tx_len = tx_len;
	/* translate WIFI field */
    for (; rx_len!=0;)
    {
        if(strncmpi(rx, "baudrate=", 9) == 0)
        { rx += 9; rx_len -= 9; baudrate = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
        else if(strncmpi(rx, "framelen=", 9) == 0)
        { rx += 9; rx_len -= 9; framelen = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }		 
        else if(strncmpi(rx, "frametime=", 10) == 0)
        { rx += 10; rx_len -= 10; frametime = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
        else
        { rx++; rx_len--; }
    }

	wl_baudrate = atoi(baudrate);
	wl_framelen = atoi(framelen);
	wl_frametime = atoi(frametime);

	/* Check WIFI parameters valid */
	if (wl_baudrate < 300 || wl_baudrate > 1250000 ||
		wl_framelen < 0 || wl_framelen > 1400 ||
		wl_frametime < 30 ) 
	{
		log_info("Uart data(setupuart) error.\n");
		return;	
	}
	log_info("baudrate:%d, framelen:%d, frametime:%d.\n", wl_baudrate, wl_framelen, wl_frametime);
	

	UserParam.Baudrate = wl_baudrate;
	UserParam.frameLength = wl_framelen;
	UserParam.frameGap = wl_frametime;

	http_sendfile(idx, "/wifinet_0x6D000.html", (unsigned char*)tx);
	/* set user parameters valid flag */		 
}

void http_setup_net(unsigned int idx, char *rx, unsigned int rx_len, char *tx)
{
    unsigned int i = 0;
	unsigned int tx_len;
	char *socktype = 0, *conntype = 0, *remoteip = 0, *port = 0,*workmode = 0;
	int wl_socktype, wl_conntype, wl_remoteip, wl_port;

    tx_len = tx_len;
	/* translate WIFI field */
    for (; rx_len!=0;)
    {
	    if(strncmpi(rx,"workmode=",9) == 0)
		{rx += 9; rx_len -= 9;  workmode = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i;}
        else if(strncmpi(rx, "socktype=", 9) == 0)
        { rx += 9; rx_len -= 9; socktype = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
        else if(strncmpi(rx, "conntype=", 9) == 0)
        { rx += 9; rx_len -= 9; conntype = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }		 
        else if(strncmpi(rx, "remoteip=", 9) == 0)
        { rx += 9; rx_len -= 9; remoteip = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
        else if(strncmpi(rx, "port=", 5) == 0)
        { rx += 5; rx_len -= 5; port = rx; i = url_decode(rx, rx, rx_len); rx += i; rx_len -= i; }
        else
        { rx++; rx_len--; }
    }

	wl_socktype = atoi(socktype);
	wl_conntype = atoi(conntype);
	inet_aton(remoteip, &wl_remoteip);
	wl_port = atoi(port);
	wl_workmode = atoi(workmode);


	/* Check WIFI parameters valid */
	if (wl_socktype < 0 || wl_socktype > 1 ||
		wl_conntype < 0 || wl_conntype > 1 ||
		wl_port > 65535 || wl_workmode < 0 || wl_workmode > 1) 
	{
		log_info("Uart data(setupuart) error.\n");
		return;	
	}
	log_info("socktype:%d, conntype:%d, remoteip:%d, port:%d.\n", wl_socktype, wl_conntype, wl_remoteip, wl_port);
	

	UserParam.socketProtocol = wl_socktype;
	UserParam.socketType = wl_conntype;
	UserParam.remoteCommIp = wl_remoteip;	 
	UserParam.socketPort = wl_port; 

	http_sendfile(idx, "/wifinet_0x6D000.html", (unsigned char *)tx);
}
