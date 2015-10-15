/*
 * =====================================================================================
 *
 *       Filename:  str_lib.c
 *
 *    Description:  Process string function
 *
 *        Version:  0.0.1
 *        Created:  2015/1/6 16:31:02
 *       Revision:  none
 *
 *         Author:  Lin Hui (Link), linhui.568@163.com
 *   Organization:  Nufront
 *
 *--------------------------------------------------------------------------------------          
 *       ChangLog:
 *        version    Author      Date          Purpose
 *        0.0.1      Lin Hui    2015/1/6      Initialize
 *
 * =====================================================================================
 */
#include "includes.h"
#include "str_lib.h"

char* nustrstr(const char *str1, const char* str2)
{
    int n;
	char *s1 = str1;
	char *s2 = str2;

    if (*s2 != NULL)
    {
        while (*s1)
        {
            for (n = 0; *(s1+n) == *(s2 + n); n++)
            {
                if (!*(s2 + n + 1))
                    return (char*)s1;
            }
            s1++;
        }
        return NULL;
    }
    else
    {
        return(char*)s1; 
    }
}


/*
 * Judge the string input is number string.
 * 0: is not a number, 1:is number string.
 **/
int str_is_digit(const char * str)
{
	int i = 0;
	int str_len;

	str_len = strlen(str);

	for (i = 0; i < str_len; i++) {
		if (!isdigit(str[i])) {
			return 0;
		}
	}

	return 1;
}

/* Check the validable of ip string.
 **/
int is_valid_ip(const char *ip) 
{ 
    int section = 0;
    int dot = 0;

    while (*ip) { 
        if (*ip == '.') { 
            dot++; 
            if (dot > 3) { 
                return -1; 
            } 

            if (section >= 0 && section <= 255) { 
                section = 0; 
            } else { 
                return -1; 
            } 
        } else if (*ip >= '0' && *ip <= '9') { 
            section = section * 10 + *ip - '0'; 
            			 
        } else { 
            return -1; 
        } 
        ip++;        
    }
	
	if (section > 0 && section <= 255)	{ 
        if (3 == dot) {
	        section = 0; 
	        return 0;
      	}
    }
		 
    return -1; 	
}


/*
 * Compare two string, if prefix is the sub string in line on the head, 
 * then return 1, else return 0.
 **/
int strStartsWith(const char *line, const char *prefix)
{
    for ( ; *line != '\0' && *prefix != '\0' ; line++, prefix++) {
        if (*line != *prefix) {
            return 0;
        }
    }

    return *prefix == '\0';
}


/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no moretokens).
 *
 * If *stringp is NULL, strsep returns NULL.
 */
char *strsep(char **stringp, const char *delim)
{
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp)== NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc =*spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
}

/*  
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.  
 * See the copyright notice in the ACK home directory, in the file "Copyright".  
 */   
/* $Id: strncmp.c,v 1.4 1994/06/24 11:57:04 ceriel Exp $ */   

int strncmp(register const char *s1, register const char *s2, register unsigned int n)   
{   
    if (n) {   
        do {   
            if (*s1 != *s2++)   
                break;   
            if (*s1++ == '\0')   
                return 0;   
        } while (--n > 0);   
        if (n > 0) {   
            if (*s1 == '\0') return -1;   
            if (*--s2 == '\0') return 1;   
            return (unsigned char) *s1 - (unsigned char) *s2;   
        }   
    }   
    return 0;   
}

char* strrmvspace(char *dst, const char *src)
{
    unsigned int i;

    if(*src == 0)
    {
        *dst = 0;
    }
    else
    {
        for (i = 0; isspace(src[i]); i++);
        strcpy(dst, &src[i]);

        i = strlen(dst) - 1;
        for (i = strlen(dst) - 1; isspace(dst[i]); i--)
        {
            dst[i] = 0;
        }
    }

    return dst;
}


char* strtoupper(char *dst, const char *src)
{
    char c;

    while(*src)
    {
        c    = *src++;
        *dst++ = toupper(c);
    }
    *dst = 0;

    return dst;
}

int strncmpi(const char *s1, const char *s2, size_t n)
{
    unsigned char c1, c2;

    if(n == 0)
    {
        return 0;
    }

    do
    {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
        if((c1 == 0) || (c2 == 0))
        {
            break;
        }
    }
    while((c1 == c2) && --n);

    return c1-c2;
}

char* utoa(unsigned val, char *buf, int radix)
{
    char *s, *p;
    s = "0123456789abcdefghijklmnopqrstuvwxyz";

    if(radix == 0)
    {
        radix = 10;
    }
    if(buf == NULL)
    {
        return NULL;
    }
    if(val < (unsigned)radix)
    {
        buf[0] = s[val];
        buf[1] = '\0';
    }
    else
    {
        for(p = utoa(val / ((unsigned)radix), buf, radix); *p; p++);
        utoa(val % ((unsigned)radix), p, radix);
    }
    return buf;
}

char* itoa(int val, char *buf, int radix)
{
    char *p;
    unsigned u;

    p = buf;
    if(radix == 0)
    {
        radix = 10;
    }
    if(buf == NULL)
    {
        return NULL;
    }
    if(val < 0)
    {
        *p++ = '-';
        u = -val;
    }
    else
    {
        u = val;
    }
    utoa(u, p, radix);

    return buf;
}

unsigned int atou_hex(const char *s)
{
    unsigned int value=0;

    if(!s)
    {
        return 0;
    }  

    while(*s)
    {
        if((*s >= '0') && (*s <= '9'))
        {
            value = (value*16) + (*s-'0');
        }
        else if((*s >= 'A') && ( *s <= 'F'))
        {
            value = (value*16) + (*s-'A'+10);
        }
        else if((*s >= 'a') && (*s <= 'f'))
        {
            value = (value*16) + (*s-'a'+10);
        }
        else
        {
            break;
        }
        s++;
    }

    return value;
}

 
