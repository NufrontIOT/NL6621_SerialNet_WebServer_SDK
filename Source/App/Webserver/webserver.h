#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#define WEB_PORT 				(80)
#define TCP_ENTRIES             (5)		/* TCP client connect number */

#define RECV_BUFFER_SIZE		(3120)
#define SEND_BUFFER_SIZE		(4096)

void WebServerThread(void * arg);

#endif
