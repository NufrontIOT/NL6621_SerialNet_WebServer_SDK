/*
 * =====================================================================================
 *
 *       Filename:  str_lib.h
 *
 *    Description:  Lib of string 
 *
 *        Version:  0.0.1
 *        Created:  2015/6/19 16:01:42
 *       Revision:  none
 *
 *         Author:  Lin Hui (Link), linhui.568@163.com
 *   Organization:  Nufront
 *
 *--------------------------------------------------------------------------------------          
 *       ChangLog:
 *        version    Author      Date          Purpose
 *        0.0.1      Lin Hui    2015/6/19      
 *
 * =====================================================================================
 */

#ifndef  __STR_LIB_H__
#define  __STR_LIB_H__

/* Check the input parameters string is digit */
int str_is_digit(const char * str);

/* check the validable of the IP string  */
int is_valid_ip(const char *ip);

/* Decompose a string into a set of strings */
char *strsep(char **stringp, const char *delim);

/* compare string between line with prefix */
int strStartsWith(const char *line, const char *prefix);

/* strstr function as in string.h */
char* nustrstr(const char *str1, const char* str2);
int strncmp(register const char *s1, register const char *s2, register unsigned int n);
char* strrmvspace(char *dst, const char *src);
char* strtoupper(char *dst, const char *src);
int strncmpi(const char *s1, const char *s2, size_t n);
char* utoa(unsigned val, char *buf, int radix);
char* itoa(int val, char *buf, int radix);
unsigned int atou_hex(const char *s);

#endif   /* ----- #ifndef __STR_LIB_H__  ----- */
