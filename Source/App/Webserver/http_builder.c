/*
 * =====================================================================================
 *
 *       Fidatalename:  http_builder.c
 *
 *    Description:  Builder of HTTP engine
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
 *        0.0.2		 Lin Hui    2015/9/24      Add server html page buidler function      
 *
 * =====================================================================================
 */
#include "includes.h"
#include "http_builder.h"
#include "str_lib.h"

/* ��ѡ�� */
#define PROTOCOL_VALUE_0		"name=\"protocol\" value=\"0\""
#define PROTOCOL_VALUE_2		"name=\"protocol\" value=\"2\""

#define ENCRY_VALUE_0			"name=\"encry\" value=\"0\""
#define ENCRY_VALUE_1			"name=\"encry\" value=\"1\""
#define ENCRY_VALUE_2			"name=\"encry\" value=\"2\""
#define ENCRY_VALUE_3			"name=\"encry\" value=\"3\""
#define ENCRY_VALUE_4			"name=\"encry\" value=\"4\""

#define AUTHMODE_VALUE_0		"name=\"authmode\" value=\"0\""
#define AUTHMODE_VALUE_1		"name=\"authmode\" value=\"1\""
#define AUTHMODE_VALUE_2		"name=\"authmode\" value=\"2\""
#define AUTHMODE_VALUE_3		"name=\"authmode\" value=\"3\""

#define WORKMODE_VALUE_0		"name=\"workmode\" value=\"0\""
#define WORKMODE_VALUE_1		"name=\"workmode\" value=\"1\""

#define SOCKTYPE_VALUE_1		"name=\"socktype\" value=\"0\""
#define SOCKTYPE_VALUE_2		"name=\"socktype\" value=\"1\""

#define CONNECTTYPE_VALUE_1		"name=\"conntype\" value=\"0\""
#define CONNECTTYPE_VALUE_2		"name=\"conntype\" value=\"1\""

#define RADIO_CHECK_DEFAULT		" checked=\"checked\" "

/* �����б�� */
#define SELECT_CHECK_DEFAULT	" selected=\"true\""
#define SELECT_OPTION_BEGIN		"option value=\""
#define SELECT_OPTION_END		"\""

#define strmerge(str1,str2)		str1##str2
#define g(a)					#a
#define h(a)					g(a)
			

extern NET_IF netif;
extern CFG_PARAM SysParam;
extern USER_CFG_PARAM UserParam;

extern UINT8   PermanentAddress[MAC_ADDR_LEN];
extern const INT8U FwType[];
extern const INT8U FwVerNum[3];
extern const INT8U AtVerNum[3];
extern int wl_workmode;


/* 
 * ע�������datalenֵΪʵ�ʵ�HTMLҳ�泤�ȣ�����HTMLҳ����Ҫ������4KBytes���ڡ�
 * */
int html_rebuild_sysstatus(unsigned char *dest, unsigned char *src, unsigned int len)
{
	unsigned char *dst = dest;
	unsigned char *mainstr = src;
	unsigned char *substr = NULL;
	unsigned int datalen = len;

	log_info("dst1 addr:%p.\n", dst);

	/* ����SDK�汾�� */
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"SDK Version:");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�SDK�汾��֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"SDK Version: %x.%02x.%02x", 		
				FwVerNum[0], FwVerNum[1], FwVerNum[2]);	   /* ����SDK version�汾��Ϣ */
		log_info("data length:%d\n", substr - mainstr);
		dst += 20;
		mainstr = substr + strlen("SDK Version:");
		datalen -= strlen("SDK Version:");
	}
	log_info("dst2 addr:%p.\n", dst);
	
	/* ����serialNet�汾�� */
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"SerialNet Version:");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�SerialNet�汾��֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"SerialNet Version: %x.%02x.%02x", 	
				AtVerNum[0], AtVerNum[1], AtVerNum[2]);	 	/* ����SerialNet version�汾��Ϣ */
		log_info("data length:%d\n", substr - mainstr);
		dst += 26;
		mainstr = substr + strlen("SerialNet Version:");
		datalen -= strlen("SerialNet Version:");
	}
	log_info("dst3 addr:%p.\n", dst);

	/* ����IP��ַ��Ϣ */
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"IP ADDR:");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�IP��ַ֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"IP ADDR: %s", 	
				inet_ntoa(netif.ip_addr.addr));	 	/* ����IP��ַ��Ϣ */
		log_info("data length:%d\n", substr - mainstr);
		dst += (9 + strlen(inet_ntoa(netif.ip_addr.addr)));
		mainstr = substr + strlen("IP ADDR:");
		datalen -= strlen("IP ADDR:");
	}
	log_info("dst4 addr:%p.\n", dst);
	
	/* ����MAC��ַ */
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"MAC ADDR:");
	if (substr != NULL) {			
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�MAC��ַ֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"MAC ADDR: %02x:%02x:%02x:%02x:%02x:%02x", 	
				PermanentAddress[0], PermanentAddress[1], PermanentAddress[2], 
				PermanentAddress[3], PermanentAddress[4], PermanentAddress[5]);	 	/* ����MAC��ַ��Ϣ */
		log_info("data length:%d\n", substr - mainstr);
		dst += 27;
		mainstr = substr + strlen("MAC ADDR:");
		datalen -= strlen("MAC ADDR:");
	}
	log_info("dst5 addr:%p.\n", dst);

	/* ����ʣ�µ�HTMLҳ������ */
	for (;;)
	{
		if (*mainstr == 0)
		{
		    break;
		}
		*dst++ = *mainstr++;
		if (datalen-- == 0)
		{
		    break;
		}
	}

	return len - datalen;
}


int html_rebuild_wifibase(unsigned char *dest, unsigned char *src, unsigned int len)
{
	unsigned char *dst = dest;
	unsigned char *mainstr = src;
	unsigned char *substr = NULL;
	unsigned char *valuestr = NULL;
	unsigned int datalen = len;
	unsigned char channel[30] = {0};
	unsigned char *pt = 0;

	log_info("dst1 addr:%p.\n", dst);

	/* ����WIFI MODE */
	sprintf((char*)channel, (char*)"%s%d%s", SELECT_OPTION_BEGIN, UserParam.cfg.WiFiCfg.Protocol, SELECT_OPTION_END);
	valuestr = channel;
	log_info("valuestr:%s.\n\n", valuestr);
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)valuestr);	
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"%s %s", valuestr, SELECT_CHECK_DEFAULT);
		log_info("data length:%d\n", substr - mainstr);
		dst += (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
		mainstr = substr + strlen((const char*)valuestr);
		datalen -= (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
	}
	log_info("dst2 addr:%p.\n", dst);

    /* ����ENCRY */
	sprintf((char*)channel, (char*)"%s%d%s", SELECT_OPTION_BEGIN, UserParam.cfg.WiFiCfg.Encry, SELECT_OPTION_END);
	valuestr = channel;
	log_info("valuestr:%s.\n\n", valuestr);
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)valuestr);	
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"%s %s", valuestr, SELECT_CHECK_DEFAULT);
		log_info("data length:%d\n", substr - mainstr);
		dst += (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
		mainstr = substr + strlen((const char*)valuestr);
		datalen -= (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
	}
	log_info("dst3 addr:%p.\n", dst);

	 /* ����AUTHMODE */
	sprintf((char*)channel, (char*)"%s%d%s", SELECT_OPTION_BEGIN, UserParam.cfg.WiFiCfg.AuthMode, SELECT_OPTION_END);
	valuestr = channel;
	log_info("valuestr:%s.\n\n", valuestr);
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)valuestr);	
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"%s %s", valuestr, SELECT_CHECK_DEFAULT);
		log_info("data length:%d\n", substr - mainstr);
		dst += (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
		mainstr = substr + strlen((const char*)valuestr);
		datalen -= (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
	}
	log_info("dst4 addr:%p.\n", dst);


	/* ����SSID */
	if(UserParam.cfg.WiFiCfg.Protocol == 0)
       pt = UserParam.web_ssid;	
	else if(UserParam.cfg.WiFiCfg.Protocol == 2)
	   pt = UserParam.cfg.WiFiCfg.Ssid;
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"apssid\" value=\"");	
	if (substr != NULL) {
        memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"apssid\" value=\"%s", pt);	   
		log_info("data length:%d\n", substr - mainstr);
		dst += (21 + strlen((const char *)pt));
		mainstr = substr + strlen("name=\"apssid\" value=\"");
		datalen -= (21 + strlen((const char *)pt));
	}
	log_info("dst2 addr:%p.\n", dst);
	
    /* ����key */
	if(UserParam.cfg.WiFiCfg.Protocol == 0)
       pt = UserParam.web_key;	
	else if(UserParam.cfg.WiFiCfg.Protocol == 2)
	   pt = UserParam.cfg.WiFiCfg.PSK;
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"apkey\" value=\"");	
	if (substr != NULL) {
        memcpy(dst, mainstr, substr - mainstr);		
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"apkey\" value=\"%s", pt);	   
		log_info("data length:%d\n", substr - mainstr);
		dst += (20 + strlen((const char *)pt));
		mainstr = substr + strlen("name=\"apkey\" value=\"");
		datalen -= (20 + strlen((const char *)pt));
	}
	log_info("dst3 addr:%p.\n", dst);	

	/* ����sta IP��ַ */
	if(UserParam.cfg.WiFiCfg.Protocol == 2)
	   pt = (unsigned char *)inet_ntoa(UserParam.cfg.IPCfg.Ip); 	
	else if(UserParam.cfg.WiFiCfg.Protocol == 0)
	   pt = (unsigned char *)inet_ntoa(UserParam.web_IPCfg.Ip);
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"apipaddr\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�IP��ַ֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"apipaddr\" value=\"%s", 		
				 pt);	   /* ����IP��ַ */
		log_info("data length:%d\n", substr - mainstr);
		dst += (23 + strlen((const char *)pt));
		mainstr = substr + strlen("name=\"apipaddr\" value=\"");
		datalen -= (23 + strlen((const char *)pt));
	}
	log_info("dst4 addr:%p.\n", dst);
	
	/* ����sta netmask */
	if(UserParam.cfg.WiFiCfg.Protocol == 2)
	   	pt = (unsigned char *)inet_ntoa(UserParam.cfg.IPCfg.Netmask);
	else if(UserParam.cfg.WiFiCfg.Protocol == 0)
	    pt = (unsigned char *)inet_ntoa(UserParam.web_IPCfg.Netmask);

	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"apnetmask\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�netmask֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"apnetmask\" value=\"%s", 		
				pt);	   /* ����netmask��Ϣ */
		log_info("data length:%d\n", substr - mainstr);
		dst += (24 + strlen((const char *)pt));
		mainstr = substr + strlen("name=\"apnetmask\" value=\"");
		datalen -= (24 + strlen((const char *)pt));
	}
	log_info("dst5 addr:%p.\n", dst);

	/* ����Channel */
	sprintf((char*)channel, (char*)"%s%d%s", SELECT_OPTION_BEGIN, UserParam.cfg.WiFiCfg.Channel, SELECT_OPTION_END);
	valuestr = channel;
	log_info("valuestr:%s.\n\n", valuestr);
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)valuestr);	
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"%s %s", valuestr, SELECT_CHECK_DEFAULT);
		log_info("data length:%d\n", substr - mainstr);
		dst += (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
		mainstr = substr + strlen((const char*)valuestr);
		datalen -= (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
	}
	log_info("dst6 addr:%p.\n", dst);



/////////////////STA

	/* ����sta SSID */
	if(UserParam.cfg.WiFiCfg.Protocol == 0)
      	pt = UserParam.cfg.WiFiCfg.Ssid; 
	else if(UserParam.cfg.WiFiCfg.Protocol == 2)
	    pt = UserParam.web_ssid;	

	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"stassid\" value=\"");	
	if (substr != NULL) {
        memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"stassid\" value=\"%s", pt);	   
		log_info("data length:%d\n", substr - mainstr);
		dst += (22 + strlen((const char *)pt));
		mainstr = substr + strlen("name=\"stassid\" value=\"");
		datalen -= (22 + strlen((const char *)pt));
	}
	log_info("ds7 addr:%p.\n", dst);
	
    /* ����sta key */
	if(UserParam.cfg.WiFiCfg.Protocol == 0)
       pt = UserParam.cfg.WiFiCfg.PSK;
	else if(UserParam.cfg.WiFiCfg.Protocol == 2)
	   pt = UserParam.web_key;	

	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"stakey\" value=\"");	
	if (substr != NULL) {
        memcpy(dst, mainstr, substr - mainstr);		
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"stakey\" value=\"%s", pt);	   
		log_info("data length:%d\n", substr - mainstr);
		dst += (21 + strlen((const char *)pt));
		mainstr = substr + strlen("name=\"stakey\" value=\"");
		datalen -= (21 + strlen((const char *)pt));
	}
	log_info("dst8 addr:%p.\n", dst);
		
 	 /* ����sta DHCP Server */
	sprintf((char*)channel, (char*)"%s%d%s", SELECT_OPTION_BEGIN, UserParam.cfg.IPCfg.bDhcp, SELECT_OPTION_END);
	valuestr = channel;
	log_info("valuestr:%s.\n\n", valuestr);
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)valuestr);	
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"%s %s", valuestr, SELECT_CHECK_DEFAULT);
		log_info("data length:%d\n", substr - mainstr);
		dst += (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
		mainstr = substr + strlen((const char*)valuestr);
		datalen -= (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
	}
	log_info("dst9 addr:%p.\n", dst);

	/* ����sta IP��ַ */
	if(UserParam.cfg.WiFiCfg.Protocol == 0)
	   pt = (unsigned char *)inet_ntoa(UserParam.cfg.IPCfg.Ip); 	
	else if(UserParam.cfg.WiFiCfg.Protocol == 2)
	   pt = (unsigned char *)inet_ntoa(UserParam.web_IPCfg.Ip);
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"staipaddr\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�IP��ַ֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"staipaddr\" value=\"%s", 		
				 pt);	   /* ����IP��ַ */
		log_info("data length:%d\n", substr - mainstr);
		dst += (24 + strlen((const char *)pt));
		mainstr = substr + strlen("name=\"staipaddr\" value=\"");
		datalen -= (24 + strlen((const char *)pt));
	}
	log_info("dst10 addr:%p.\n", dst);
	
	/* ����sta netmask */
	if(UserParam.cfg.WiFiCfg.Protocol == 0)
	   	pt = (unsigned char *)inet_ntoa(UserParam.cfg.IPCfg.Netmask);
	else if(UserParam.cfg.WiFiCfg.Protocol == 2)
	    pt = (unsigned char *)inet_ntoa(UserParam.web_IPCfg.Netmask);

	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"stanetmask\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�netmask֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"stanetmask\" value=\"%s", 		
				pt);	   /* ����netmask��Ϣ */
		log_info("data length:%d\n", substr - mainstr);
		dst += (25 + strlen((const char *)pt));
		mainstr = substr + strlen("name=\"stanetmask\" value=\"");
		datalen -= (25 + strlen((const char *)pt));
	}
	log_info("dst11 addr:%p.\n", dst);

	memset(channel,0,sizeof(channel));

	/* ����gateway */
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"gateway\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�gateway֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"gateway\" value=\"%s", 		
				inet_ntoa(UserParam.web_IPCfg.GateWay));	   /* ����gateway��Ϣ */
		log_info("data length:%d\n", substr - mainstr);
		dst += (22 + strlen(inet_ntoa(UserParam.web_IPCfg.GateWay)));
		mainstr = substr + strlen("name=\"gateway\" value=\"");
		datalen -= (22 + strlen(inet_ntoa(UserParam.web_IPCfg.GateWay)));
	}
	log_info("dst12 addr:%p.\n", dst);
	
	/* ����sta DNS server */
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"dnsserv\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);		/* ���޸�DNS server֮ǰ���ݽ��п��� */
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"dnsserv\" value=\"%s", 		
				inet_ntoa(UserParam.web_IPCfg.Dns));	   /* ����DNS server��Ϣ */
		log_info("data length:%d\n", substr - mainstr);
		dst += (22 + strlen(inet_ntoa(UserParam.web_IPCfg.Dns)));
		mainstr = substr + strlen("name=\"dnsserv\" value=\"");
		datalen -= (22 + strlen(inet_ntoa(UserParam.web_IPCfg.Dns)));
	}
	log_info("dst13 addr:%p.\n", dst);

	/* ����ʣ�µ�HTMLҳ������ */
	for (;;)
	{
		if (*mainstr == 0)
		{
		    break;
		}
		*dst++ = *mainstr++;
		if (datalen-- == 0)
		{
		    break;
		}
	}

	return len - datalen;
}

int html_rebuild_wifinet(unsigned char *dest, unsigned char *src, unsigned int len)
{
	unsigned char *dst = dest;
	unsigned char *mainstr = src;
	unsigned char *substr = NULL;
	unsigned char *valuestr = NULL;
	unsigned int datalen = len;
	char digital[30];

	log_info("dst1 addr:%p.\n", dst);

	/*���벨����*/
	sprintf((char*)digital, (char*)"%s%d%s", SELECT_OPTION_BEGIN,  UserParam.Baudrate, SELECT_OPTION_END);
	valuestr = (unsigned char *)digital;
	log_info("valuestr:%s.\n\n", valuestr);
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)valuestr);	
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"%s %s", valuestr, SELECT_CHECK_DEFAULT);
		log_info("data length:%d\n", substr - mainstr);
		dst += (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
		mainstr = substr + strlen((const char*)valuestr);
		datalen -= (strlen((const char*)valuestr) + strlen((const char*)SELECT_CHECK_DEFAULT) + 1);
	}
	log_info("dst1 addr:%p.\n", dst);


	/*����֡��*/
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"framelen\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"framelen\" value=\"%d", UserParam.frameLength); 
		log_info("data length:%d\n", substr - mainstr);
		dst += (23 + strlen(itoa(UserParam.frameLength, digital, 0)));
		mainstr = substr + strlen("name=\"framelen\" value=\"");
		datalen -= (23 + strlen(itoa(UserParam.frameLength, digital, 0)));
	}
	log_info("dst2 addr:%p.\n", dst);

	/*����֡����*/
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"frametime\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"frametime\" value=\"%d", UserParam.frameGap);
		log_info("data length:%d\n", substr - mainstr);
		dst += (24 + strlen(itoa(UserParam.frameGap, digital, 0)));
		mainstr = substr + strlen("name=\"frametime\" value=\"");
		datalen -= (24 + strlen(itoa(UserParam.frameGap, digital, 0)));
	}
	log_info("dst3 addr:%p.\n", dst);

	/* ����workmode���� */	
    if (wl_workmode == 0)		/*UserParam.atMode*/	/* ��ȡ���� */
		valuestr = WORKMODE_VALUE_0;
    else if (wl_workmode == 1)
		valuestr = WORKMODE_VALUE_1;
    
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)valuestr);	
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"%s %s", valuestr, RADIO_CHECK_DEFAULT);
		log_info("data length:%d\n", substr - mainstr);
		dst += (strlen((const char*)valuestr) + strlen((const char*)RADIO_CHECK_DEFAULT) + 1);
		mainstr = substr + strlen((const char*)valuestr);
		datalen -= (strlen((const char*)valuestr) + strlen((const char*)RADIO_CHECK_DEFAULT) + 1);
	}
	log_info("dst4 addr:%p.\n", dst);


	/* ����socket���� */	
    if (UserParam.socketProtocol == 0)			/* ��ȡ���� */
		valuestr = SOCKTYPE_VALUE_1;
    else if (UserParam.socketProtocol == 1)
		valuestr = SOCKTYPE_VALUE_2;
    
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)valuestr);	
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"%s %s", valuestr, RADIO_CHECK_DEFAULT);
		log_info("data length:%d\n", substr - mainstr);
		dst += (strlen((const char*)valuestr) + strlen((const char*)RADIO_CHECK_DEFAULT) + 1);
		mainstr = substr + strlen((const char*)valuestr);
		datalen -= (strlen((const char*)valuestr) + strlen((const char*)RADIO_CHECK_DEFAULT) + 1);
	}
	log_info("dst5 addr:%p.\n", dst);

	/* ����connect���� */	
    if (UserParam.socketType == 0)			/* ��ȡ���� */
		valuestr = CONNECTTYPE_VALUE_1;
    else if (UserParam.socketType == 1)
		valuestr = CONNECTTYPE_VALUE_2;
    
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)valuestr);	
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"%s %s", valuestr, RADIO_CHECK_DEFAULT);
		log_info("data length:%d\n", substr - mainstr);
		dst += (strlen((const char*)valuestr) + strlen((const char*)RADIO_CHECK_DEFAULT) + 1);
		mainstr = substr + strlen((const char*)valuestr);
		datalen -= (strlen((const char*)valuestr) + strlen((const char*)RADIO_CHECK_DEFAULT) + 1);
	}
	log_info("dst6 addr:%p.\n", dst);

	/*����Զ��IP��ַ*/
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"remoteip\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"remoteip\" value=\"%s", inet_ntoa(UserParam.remoteCommIp));
		log_info("data length:%d\n", substr - mainstr);
		dst += (23 + strlen(inet_ntoa(UserParam.remoteCommIp)));
		mainstr = substr + strlen("name=\"remoteip\" value=\"");
		datalen -= (23 + strlen(inet_ntoa(UserParam.remoteCommIp)));
	}
	log_info("dst7 addr:%p.\n", dst);

	/*���ö˿�*/
	substr = (unsigned char*)nustrstr((const char*)mainstr, (const char*)"name=\"port\" value=\"");
	if (substr != NULL) {
		memcpy(dst, mainstr, substr - mainstr);	
		dst += (substr - mainstr);
		datalen -= (substr - mainstr);
		sprintf((char*)dst, (char*)"name=\"port\" value=\"%d", UserParam.socketPort);
		log_info("data length:%d\n", substr - mainstr);
		dst += (19 +  strlen(itoa(UserParam.socketPort, digital, 0)));
		mainstr = substr + strlen("name=\"port\" value=\"");
		datalen -= (19 +  strlen(itoa(UserParam.socketPort, digital, 0)));
	}
	log_info("dst8 addr:%p.\n", dst);


	/* ����ʣ�µ�HTMLҳ������ */
	for (;;)
	{
		if (*mainstr == 0)
		{
		    break;
		}
		*dst++ = *mainstr++;
		if (datalen-- == 0)
		{
		    break;
		}
	}

	return len - datalen;
}

