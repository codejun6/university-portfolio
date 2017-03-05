
#ifndef _MSJC_IO_H_
#define _MSJC_IO_H_

char *readintbuf(char *buf, int *len, int *dest);
char *writeintbuf(char *buf, int *len, int i);
char *readstrbuf(char *buf, int *len, char *dest, int n);
char *writestrbuf(char *buf, int *len, const char *str, int n);

#endif
