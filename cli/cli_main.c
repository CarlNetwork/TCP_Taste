/*
*version 02
*add the I/O
*add the fork()
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int 
Do_Something(FILE *fp, int sockfd);
void
Display_Sock_Peer_Name(int sockfd);

char buff[1024], recevline[1024];

int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;	
/***************************************************************/
	if( (argc == 2) && (argv[1] == "--help") ){
		printf("format:%s IPv4Addr\n",argv[0]);
		exit(0);
	}else{
		if( argc != 2 ){
			printf("Please enter:%s --help\n",argv[0]);
			exit(0);
		}		
	}
/***************************************************************/
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if( sockfd < 0 ){
		exit(-1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1024);
	if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0 ){
		printf("inet error\n");
		exit(-1);
	}
	
#if 0
	bind();
#endif

	if( connect(sockfd,(struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
		perror("connect error");
		close(sockfd);  //
		exit(-1);
	}else{
		printf("connect successful\n");
	}
	/*******************************************************************/
	Display_Sock_Peer_Name(sockfd);
	Do_Something(stdin, sockfd);
	exit(0);
			
}

int 
Do_Something(FILE *fp, int sockfd)
{
	int w_byte, r_byte, count;
	if( fgets(buff, sizeof(buff)+1, fp) != NULL ){
		w_byte = write(sockfd, buff, strlen(buff));
		if( w_byte < 0 ){
			printf("write failed\n");
			exit(-1);
		}else{
			count = strlen(buff);
			do{
				r_byte = read(sockfd, recevline, count);
				count -= r_byte;
			}
			while( r_byte > 0 );
			if( fputs(recevline, stdout) == EOF ){
				printf("fputs failed\n");
				exit(-1);
			}
		}
	}
	return 0;  //successful
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
