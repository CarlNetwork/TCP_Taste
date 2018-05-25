/*version 01*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "mylib.h"

typedef void Fun_Sig(int);

void
Display_Sock_Peer_Name(int sockfd);
void
Child_Done(int connfd);
void 
SIGCHLD_handler_wait(int sig);
void 
SIGCHLD_handler_waitpid(int sig);
Fun_Sig *
Signal(int sig, Fun_Sig *hanlder);

char recline[MAXBUFFSIZE];

int
main(int argc, char *argv[])
{
	int listenfd, connfd,fd;
	struct sockaddr_in servaddr;
	pid_t child_pid;
/***************************************************************/
	if( argc != 1 ){
		printf("Format:%s\n",argv[0]);
		exit(0);
	}
/***************************************************************/
	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	fd = socket(PF_INET, SOCK_STREAM, 0);
	if( listenfd < 0 ){
		printf("error:socket failed!\n");
		exit(-1);
	} 
	
	int val = 1;
        int ret ;
	ret= setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(void *)&val,sizeof(int));
      	if(ret == -1)
      	{
       	  printf("setsockopt");
       	  exit(1);
     	}    
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(void *)&val,sizeof(int));
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1024);		//time port
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  //*
#if 1 
	if( inet_pton(AF_INET, "192.168.47.128", &servaddr.sin_addr) < 0 ){
		printf("inet error\n");
		exit(-1);
	}
#endif
	
	if( bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
		printf("error:bind failed\n");
		perror("bind error");  //sudo 13
		exit(-1);
	}

	if( listen(listenfd, 5) < 0 ){
		printf("error1:listen failed\n");
		exit(-1);
	} 
	Signal(SIGCHLD, &SIGCHLD_handler_waitpid);

	for(;;){
		printf("start accept......\n");
		if( (connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) < 0 ){
			if( errno == EINTR ){
				printf("accept EINTR\n");				
				continue;
			}	
			else{
				printf("accept faild\n");
				exit(-1);
			}
		}
		
		printf("accept successful\n");

		/*********************************************/
		Display_Sock_Peer_Name(connfd);
#if 1
		child_pid = fork();
		switch( child_pid ){
			case -1:{//error
					printf("fork failed\n");
					exit(-1);			
				}break;
			case 0:{//child
					close(listenfd);
					Child_Done(connfd);
					close(connfd);
					exit(0);  //kill the child		
				}break;
			default:sleep(1);break;   //father
		}
		close(connfd);	
#endif
	}
			
}

Fun_Sig *
Signal(int sig, Fun_Sig *handler)
{
	struct sigaction sigc, oldsigc;
	sigset_t sigmask;

	sigemptyset(&sigmask);
	
	sigc.sa_handler = handler;
	sigc.sa_mask = sigmask;
	sigc.sa_flags = 0;
	if( sig == SIGALRM ){
#ifdef SA_INTERRUPT
		sigc.sa_flags |= SA_INTERRUPT;
#endif	
	}else{
#ifdef SA_RESTART
		sigc.sa_flags |= SA_RESTART;
#endif
	}
	if( sigaction(sig, &sigc, &oldsigc) < 0 ){
		printf("sigaction error\n");
		exit(-1);
	}
	return oldsigc.sa_handler;
}

void 
SIGCHLD_handler_wait(int sig)
{
	pid_t pid;
	int stat;

	pid = wait(&stat);
	printf("child %d have died!\n",pid);  //

	return ; 	
}

void
SIGCHLD_handler_waitpid(int sig)
{
	pid_t pid;
	int status;
	int save_errno;

	save_errno = errno;
	while( pid = waitpid(-1, &status, WNOHANG) > 0 )
		continue;

	if( (pid == -1) && (errno != ECHILD) ){
		perror("waitpid error");
		exit(-1);	
	}

	errno = save_errno;

	return ;
}


void
Child_Done(int connfd)
{
	size_t n;
	n  = 0;
lab:
	while( (n = read(connfd, recline, MAXBUFFSIZE) ) > 0 ){
		//printf("n = %d\n",(int)n);
		Write(connfd, recline, n);
		//printf("n = %d\n",(int)n);
	}
	if( n < 0 && errno == EINTR){
		goto lab;
	}else{
		if( n < 0 ){
			printf("read\n");
			exit(-1);
		}
	}
}

void
Display_Sock_Peer_Name(int sockfd)
{
	struct sockaddr_in sockname,peername;
	socklen_t socklen,peerlen;
	char sockipstr[16],peeripstr[16];

	socklen = sizeof(sockname);
	peerlen = sizeof(peername);
	if( getsockname(sockfd, (struct sockaddr *)&sockname, &socklen) < 0 ){
		printf("getsockname failed\n");
		exit(-1);
	}
	if( getpeername(sockfd, (struct sockaddr *)&peername, &peerlen) < 0 ){
		printf("getpeername failed\n");
		exit(-1);
	}
	
	if( inet_ntop(AF_INET, &sockname.sin_addr, sockipstr, sizeof(sockipstr) ) == NULL){
		printf("inet_ntop sockname failed\n");
		exit(-1);
	}
	if( inet_ntop(AF_INET, &peername.sin_addr, peeripstr, sizeof(peeripstr)) == NULL){
		printf("inet_ntop peername failed\n");
		exit(-1);
	}
	printf("getsockname:ip:%s\tport%d\n",sockipstr,ntohs(sockname.sin_port));
	printf("getpeername:ip:%s\tport%d\n",peeripstr,ntohs(peername.sin_port));
}
