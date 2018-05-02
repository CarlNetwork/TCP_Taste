/*version 01*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int
main(int argc, char *argv[])
{
	int listenfd;
	struct sockaddr_in servaddr;
/***************************************************************/
	if( argc != 1 ){
		printf("Format:%s\n",argv[0]);
		exit(0);
	}
/***************************************************************/
	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	if( listenfd < 0 ){
		printf("error:socket failed!\n");
		exit(-1);
	} 
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1024);		//time port
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  //*
	if( bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
		printf("error:bind failed\n");
		perror("bind error");  //sudo 13
		exit(-1);
	}

	if( listen(listenfd, 5) < 0 ){
		printf("error:listen failed\n");
		exit(-1);
	} 
	int connfd;
	struct sockaddr_in sockname,peername;
	socklen_t socklen,peerlen;
	char sockipstr[16],peeripstr[16];

	for(;;){
		printf("start accept......\n");
		connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
		if( connfd < 0 ){
			printf("accept faild\n");
			exit(-1);
		}else{
			printf("accept successful\n");
		}

		/*********************************************/
		
	
		socklen = sizeof(sockname);
		peerlen = sizeof(peername);
		if( getsockname(connfd, (struct sockaddr *)&sockname, &socklen) < 0 ){
			printf("getsockname failed\n");
			exit(-1);
		}
		if( getpeername(connfd, (struct sockaddr *)&peername, &peerlen) < 0 ){
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
		close(connfd);			
	}
			
}
