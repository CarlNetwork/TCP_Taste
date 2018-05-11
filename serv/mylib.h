#ifndef __MYLIB_H__
#define __MYLIB_H__

extern int 
Write(int sockfd, const void *buf, size_t n);
extern int 
Read(int sockfd, void *buf, size_t n);

#define MAXBUFFSIZE 4096

#endif
