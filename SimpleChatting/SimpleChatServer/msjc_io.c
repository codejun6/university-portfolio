
#ifndef _MSJC_IO_H_
#define _MSJC_IO_H_

#include "msjc_io.h"
#include <string.h>

char *readintbuf(char *buf, int *len, int *dest)
{
    if(dest != NULL)
        *dest = *(int *)buf;
    if(len != NULL)
        *len += sizeof(int);
    
    return buf + sizeof(int);
}

char *writeintbuf(char *buf, int *len, int i)
{
    *(int *)buf = i;
    if(len != NULL)
        *len += sizeof(int);
    
    return buf + sizeof(int);
}

char *readstrbuf(char *buf, int *len, char *dest, int n)
{
    if(dest != NULL)
        strncpy(dest, buf, n);
    if(len != NULL)
        *len += n;
    
    return buf + n;
}

char *writestrbuf(char *buf, int *len, const char *str, int n)
{
    if(n > 0)
        strncpy(buf, str, n);
    if(len != NULL)
        *len += n;
    
    return buf + n;
}

#endif
