#include <unistd.h>
#include <errno.h>
#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>

int 
Write(int sockfd, const void *buf, size_t n)
{
	ssize_t w_byte;
	size_t nleft;
	const char *ptr;
	int save_errno;

	save_errno = errno;
	ptr = buf;	
	nleft = n;	
	while( nleft > 0){
		if( (w_byte = write(sockfd, ptr, nleft)) <= 0 ){
			if( (w_byte < 0) && (errno == EINTR) )
				w_byte = 0;
			else{
				printf("write errno\n");
				exit(-1);
			}
		}
//		printf("w1 = %d  n1 = %d\n",(int)w_byte,(int)nleft);
		nleft -= w_byte;
//		printf("w1 = %d  n1 = %d\n",(int)w_byte,(int)nleft);
		ptr += w_byte;	
	}
	errno = save_errno;

	return n;	
}
int 
Read(int sockfd, void *buf, size_t n)
{
	ssize_t r_byte;
	size_t nleft;
	char *ptr;
	int save_errno;

	save_errno = errno;
	errno = 0;
	ptr = buf;
	nleft = n;
	while( nleft > 0 ){
		if( (r_byte = read(sockfd, ptr, nleft) ) < 0 ){
			//printf("s4\n");
			if( errno == EINTR )
				r_byte = 0;
			else{
				return -1;
			}
		}else{
			//printf("s5\n");
			//printf("r_byte = %d\n",(int)r_byte);
			if( r_byte == 0 )  //EOF  or  terminal off
				break;	
		}
		nleft -= r_byte;
		ptr += r_byte;
		if( (nleft > 0) && (nleft < n) ){
			break;
		}
	}
        errno = save_errno;
//	printf("n-nleft= %d\n",(int)(n-nleft));
	return (n-nleft);
}
/*
int 
Read(int sockfd, void *buf, size_t n)
{
	ssize_t r_byte;
	size_t nleft;
	char *ptr;
	int save_errno;

	save_errno = errno;
	errno = 0;
	ptr = buf;
	nleft = n;
	while( nleft > 0 ){
		r_byte = read(sockfd, ptr, nleft);
		if( r_byte < 0){
			if( errno == EINTR )
				continue;
			else{
				return -1;
			}
		}
		
		nleft -= r_byte;
		ptr += r_byte;
	}
	errno = save_errno;

	return (n-nleft);
}*/
