#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__


#define APPNAME                        	"login"//"WebRadio" //max 15 characters


#define USER_PARAMETERS_ADDR			(0x68000)
#define PUBLIC_CSS_FORMAT_ADDR			(0x69000)
#define HTML_LOGIN_ADDR					(0x6A000)
#define HTML_SYS_STATUS_ADDR			(0x6B000)
#define HTML_WIFI_SETUP_ADDR			(0x6C000)
#define HTML_WIFI_NET_ADDR				(0x6D000)


//----- DEFINES -----
#define UNKNOWN_FILE                   (0)
#define HTML_FILE                      (1)
#define XML_FILE                       (2)
#define JS_FILE                        (3)
#define CSS_FILE                       (4)
#define TXT_FILE                       (5)
#define ICON_FILE                      (6)
#define GIF_FILE                       (7)
#define JPEG_FILE                      (8)

typedef void*(*HtmlGetFunc)(void);

typedef struct
{
  const char *name;
  const unsigned int type;
  unsigned char* (*data)();
  unsigned int (*len)();
} HTTPFILE;

typedef struct
{
  const char *name; //name
  const unsigned int format; //format
  HtmlGetFunc get; 
} HTTPVAR;

//----- PROTOTYPES -----
unsigned int http_fparse(char *dst, unsigned int file, unsigned int *start, unsigned int len, unsigned int param);
unsigned int http_fdata(unsigned char *dst, unsigned int file, unsigned int start, unsigned int len);
unsigned int http_flen(unsigned int file, unsigned int param);
unsigned int http_ftype(unsigned int file);
unsigned int http_fid(const char *name, unsigned int *param);

void http_senddata_alloc(void);

unsigned int url_decode(char *dst, const char *src, unsigned int len);
char* http_skiphd(const char *src, unsigned int *len);

int http_resetsys(unsigned int idx, char *tx, unsigned int tx_len);
int http_factory(unsigned int idx, char *tx, unsigned int tx_len);

void http_setup_wifibase(unsigned int idx, char *rx, unsigned int rx_len, char *tx);
void http_setup_uart(unsigned int idx, char *rx, unsigned int rx_len, char *tx);
void http_setup_net(unsigned int idx, char *rx, unsigned int rx_len, char *tx);	


#endif //__HTTP_PARSER_H__
