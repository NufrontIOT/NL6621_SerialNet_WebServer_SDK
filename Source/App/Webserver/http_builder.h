/*
 * =====================================================================================
 *
 *       Filename:  http_builder.h
 *
 *    Description:  Head file of HTTP builder
 *
 *        Version:  0.0.1
 *        Created:  2015/6/12 17:29:46
 *       Revision:  none
 *
 *         Author:  Lin Hui (Link), linhui.568@163.com
 *   Organization:  Nufront
 *
 *--------------------------------------------------------------------------------------          
 *       ChangLog:
 *        version    Author      Date          Purpose
 *        0.0.1      Lin Hui    2015/6/12      
 *           
 * =====================================================================================
 */
#ifndef __HTTP_BUILDER_H__
#define __HTTP_BUILDER_H__


#define HTTP_HTML_HEADER "" \
  "HTTP/1.1 200 OK\r\n" \
  "Server: "APPNAME"\r\n" \
  "Connection: Close\r\n" \
  "Cache-Control: no-store, no-cache, max-age=0\r\n" \
  "Pragma: no-cache\r\n" \
  "Content-Type: text/html; charset=gb2312 \r\n" \
  "Content-Length: "

#define HTTP_JS_HEADER "" \
  "HTTP/1.1 200 OK\r\n" \
  "Server: "APPNAME"\r\n" \
  "Connection: Close\r\n" \
  "Cache-Control: no-store, no-cache, max-age=86400\r\n" \
  "Content-Type: application/x-javascript\r\n" \
  "Content-Length: "

#define HTTP_CSS_HEADER "" \
  "HTTP/1.1 200 OK\r\n" \
  "Server: "APPNAME"\r\n" \
  "Connection: Close\r\n" \
  "Cache-Control: no-store, no-cache, max-age=86400\r\n" \
  "Content-Type: text/css\r\n" \
  "Content-Length: "

#define HTTP_ICON_HEADER "" \
  "HTTP/1.1 200 OK\r\n" \
  "Server: "APPNAME"\r\n" \
  "Connection: Close\r\n" \
  "Cache-Control: no-store, no-cache, max-age=86400\r\n" \
  "Content-Type: image/x-icon\r\n" \
  "Content-Length: "

#define HTTP_200_HEADER "" \
  "HTTP/1.1 200 OK\r\n" \
  "Server: "APPNAME"\r\n" \
  "Connection: Close\r\n" \
  "Cache-Control: no-store, no-cache, max-age=0\r\n" \
  "Pragma: no-cache\r\n" \
  "Content-Type: text/html; charset=gb2312 \r\n" \
  "Content-Length: 0\r\n\r\n"
//  "HTTP/1.1 200 OK\r\n" \
//  "Server: "APPNAME"\r\n" \
//  "Connection: Close\r\n" \
//  "\r\n"

#define HTTP_400_HEADER "" \
  "HTTP/1.1 400 Bad request\r\n" \
  "Server: "APPNAME"\r\n" \
  "Connection: Close\r\n" \
  "\r\n"

#define HTTP_404_HEADER "" \
  "HTTP/1.1 404 Not found\r\n" \
  "Server: "APPNAME"\r\n" \
  "Connection: Close\r\n" \
  "\r\n"


int html_rebuild_sysstatus(unsigned char *dst, unsigned char *src, unsigned int len);
int html_rebuild_wifibase(unsigned char *dest, unsigned char *src, unsigned int len);
int html_rebuild_wifinet(unsigned char *dest, unsigned char *src, unsigned int len);

#endif

