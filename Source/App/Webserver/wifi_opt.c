#include "common.h"

#include "http_engine.h"
#include "http_parser.h"

#include "wifi_opt.h"
#include "str_lib.h"



CFG_PARAM WifiUserParam;

const char *AuthModeStr[PARAM_AUTH_MODE_MAX] = {"open",  "share",  "wpapsk",  "wpa2psk"};
const char *EncModeStr[PARAM_ENC_MODE_MAX] = {"open",  "web",  "tkip",  "ccmp", "auto"};

//Louis add 
extern VOID SaveUserParam(VOID);

extern USER_CFG_PARAM FactoryParam;
//出厂设置
void web_config_init(void)
{
    memset(FactoryParam.web_ssid,0,32);
    memset(FactoryParam.web_key,0,64);
	memset(&FactoryParam.web_IPCfg,0,sizeof(IP_CFG));

	memcpy(FactoryParam.web_ssid,"nufront_sta",strlen("nufront_sta"));
	memcpy(FactoryParam.web_key,"nufront_sta",strlen("nufront_sta"));

	// ip config
	FactoryParam.web_IPCfg.bDhcp = 1;
	FactoryParam.web_IPCfg.DhcpTryTimes = 5;
	FactoryParam.web_IPCfg.Ip[0] = 192;
	FactoryParam.web_IPCfg.Ip[1] = 168;
	FactoryParam.web_IPCfg.Ip[2] = 0;
	FactoryParam.web_IPCfg.Ip[3] = 1;
	FactoryParam.web_IPCfg.Netmask[0] = 255;
	FactoryParam.web_IPCfg.Netmask[1] = 255;
	FactoryParam.web_IPCfg.Netmask[2] = 255;
	FactoryParam.web_IPCfg.Netmask[3] = 0;
	FactoryParam.web_IPCfg.GateWay[0] = 192;
	FactoryParam.web_IPCfg.GateWay[1] = 168;
	FactoryParam.web_IPCfg.GateWay[2] = 0;
	FactoryParam.web_IPCfg.GateWay[3] = 1;
	FactoryParam.web_IPCfg.Dns[0] = 192;
	FactoryParam.web_IPCfg.Dns[1] = 168;
	FactoryParam.web_IPCfg.Dns[2] = 0;
	FactoryParam.web_IPCfg.Dns[3] = 1;
}

char*  wl_get_ssid(void)
{
    return (char*)SysParam.WiFiCfg.Ssid;
}


char WlKeyBuf[65] = {0};
char* wl_get_key(void)
{
    if (SysParam.WiFiCfg.KeyLength > 64)
        return NULL;

    memcpy((unsigned char*)WlKeyBuf, SysParam.WiFiCfg.PSK, SysParam.WiFiCfg.KeyLength);
    WlKeyBuf[SysParam.WiFiCfg.KeyLength] = 0;
    
    return WlKeyBuf;
}

char* wl_get_protocol(void)
{
	return (char*)SysParam.WiFiCfg.Protocol;
}


char* wl_get_authmode(void)
{
    switch (SysParam.WiFiCfg.AuthMode)
    {
        case PARAM_AUTH_MODE_OPEN:
        case PARAM_AUTH_MODE_SHARE:
        case PARAM_AUTH_MODE_WPAPSK:
        case PARAM_AUTH_MODE_WPA2PSK:
        	return AuthModeStr[SysParam.WiFiCfg.AuthMode];

        default:
        	return NULL;
    }
}

char* wl_get_encrymode(void)
{
    switch (SysParam.WiFiCfg.Encry)
    {
        case PARAM_ENC_MODE_OPEN:
        case PARAM_ENC_MODE_WEP:
        case PARAM_ENC_MODE_TKIP:
        case PARAM_ENC_MODE_CCMP:
        case PARAM_ENC_MODE_AUTO:
        	return EncModeStr[SysParam.WiFiCfg.Encry];

        default:
        	return NULL;
    }
}


int wl_get_channel(void) 
{
    return SysParam.WiFiCfg.Channel;
}



void wl_set_ssid(char *ssid)
{
    if (strlen(ssid) > 32)
        ssid[32-1] = 0; 
    
    InfSsidSet((UINT8 *)ssid,  strlen(ssid));  
}


void wl_set_key(char *key)
{
    if (strlen(key) > KEY_LEN)
        key[KEY_LEN] = 0;
    
    InfKeySet(0, (unsigned char *)key, strlen(key));   
}

void wl_set_ipconfig(unsigned int ipaddr, unsigned int netmask, unsigned int gateway, unsigned int dnsserv)
{
	UserParam.cfg.IPCfg.Ip[0] = ip4_addr1(&ipaddr);
	UserParam.cfg.IPCfg.Ip[1] = ip4_addr2(&ipaddr);
	UserParam.cfg.IPCfg.Ip[2] = ip4_addr3(&ipaddr);
	UserParam.cfg.IPCfg.Ip[3] = ip4_addr4(&ipaddr);

	UserParam.cfg.IPCfg.Netmask[0] = ip4_addr1(&netmask);
	UserParam.cfg.IPCfg.Netmask[1] = ip4_addr2(&netmask);
	UserParam.cfg.IPCfg.Netmask[2] = ip4_addr3(&netmask);
	UserParam.cfg.IPCfg.Netmask[3] = ip4_addr4(&netmask);
	
	UserParam.cfg.IPCfg.GateWay[0] = ip4_addr1(&gateway);
	UserParam.cfg.IPCfg.GateWay[1] = ip4_addr2(&gateway);
	UserParam.cfg.IPCfg.GateWay[2] = ip4_addr3(&gateway);
	UserParam.cfg.IPCfg.GateWay[3] = ip4_addr4(&gateway);

	UserParam.cfg.IPCfg.Dns[0] = ip4_addr1(&dnsserv);
	UserParam.cfg.IPCfg.Dns[1] = ip4_addr2(&dnsserv);
	UserParam.cfg.IPCfg.Dns[2] = ip4_addr3(&dnsserv);
	UserParam.cfg.IPCfg.Dns[3] = ip4_addr4(&dnsserv);
}

//#define  WIFI_CONFIG_DATA_ADDR			(0x66000)
//
//void GetWifiParam(CFG_PARAM *wconf)
//{
////	ReadFlash((UINT8 *)wconf, sizeof(CFG_PARAM), WIFI_CONFIG_DATA_ADDR);
//}
//
//void SaveWifiParam(void)
//{
//	memset(&WifiUserParam, 0, sizeof(CFG_PARAM));
//	memcpy(&WifiUserParam, &SysParam, sizeof(CFG_PARAM));
//
////	WriteFlash((UINT8*)&WifiUserParam, sizeof(CFG_PARAM), WIFI_CONFIG_DATA_ADDR);
//
//	/* Save SSID and password to nor flash */
////	GAgent_SaveSSIDAndKey((char*)SysParam.WiFiCfg.Ssid, (char*)SysParam.WiFiCfg.PSK);
//}


void SetWifiParam(char *ssid, char *key,
				unsigned int ipaddr,
				unsigned int netmask,
				unsigned int gateway,
				unsigned int dnsserv, 
				unsigned int protocol, 
				unsigned int encrymode,
				unsigned int authmode,
				unsigned int channel)
{
	wl_set_ipconfig(ipaddr, netmask, gateway, dnsserv);

	UserParam.cfg.WiFiCfg.Protocol = protocol;

    if(protocol == 0)
	{
	   memcpy(UserParam.web_IPCfg.GateWay,UserParam.cfg.IPCfg.GateWay,4); 
	   memcpy(UserParam.web_IPCfg.Dns,UserParam.cfg.IPCfg.Dns,4); 
	}
	//ssid
	NST_MOVE_MEM(UserParam.cfg.WiFiCfg.Ssid,ssid,strlen(ssid));
   	UserParam.cfg.WiFiCfg.Ssid[strlen(ssid)] = 0;
	//key
	NST_MOVE_MEM(UserParam.cfg.WiFiCfg.PSK,key,strlen(key));
	if (strlen(key) < 64)
		UserParam.cfg.WiFiCfg.PSK[strlen(key)] = 0;
    UserParam.cfg.WiFiCfg.KeyLength = strlen(key);
	//加密方式			
	UserParam.cfg.WiFiCfg.AuthMode =  (PARAM_AUTH_MODE)authmode;
	UserParam.cfg.WiFiCfg.Encry = (PARAM_ENC_MODE)encrymode;

	if (channel > 0 && channel < 14)
	  UserParam.cfg.WiFiCfg.Channel = channel;
}


