#ifndef _HTTP_H_
#define _HTTP_H_


#define HTTP_CLOSED                    	(0)
#define HTTP_SEND                      	(1)
#define HTTP_LOGIN                     	(2)


typedef struct
{
	int		   sock_fd;
	unsigned int status;
	unsigned int file;
	unsigned int ftype;
	unsigned int flen;
	unsigned int fpos;
	unsigned int fparse;
	unsigned int fparam;
} HTTP_Table;

void tcp_send(unsigned int idx, unsigned int len);

void http_close(unsigned int idx);
void http_engine(unsigned int idx, char *rx, unsigned int rx_len, unsigned char *tx);

void WebServerThread(void * arg);


#endif //_HTTP_H_
