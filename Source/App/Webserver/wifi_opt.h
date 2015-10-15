#ifndef _WLCFG_H_
#define _WLCFG_H_

/*以下涉及到网页中长度的宏定义，在网页中输入最大长度maxlength也进行了相同的
限定，具体可以去http_file.c的HTM常量数组中查找maxlength关键字对应。*/
#define KEY_LEN 64
#define STR_LEN 16


char* wl_get_ssid(void);
char* wl_get_key(void);

char* wl_get_protocol(void); 
char* wl_get_encrymode(void);
char* wl_get_authmode(void);
int wl_get_channel(void); 

void wl_set_ssid(char *ssid);
void wl_set_key(char *key);
void SetWifiParam(char *ssid, char *key,
				unsigned int ipaddr,
				unsigned int netmask,
				unsigned int gateway,
				unsigned int dnsserv, 
				unsigned int protocol, 
				unsigned int encrymode,
				unsigned int authmode,
				unsigned int channel);

void web_config_init(void);

void web_to_wifi(void);

#endif // _WLCFG_H_
