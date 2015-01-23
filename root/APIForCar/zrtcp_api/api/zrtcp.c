#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string.h>


#include "zrtcp.h"

#define LISTEN_LIMIT 1

//static struct sockaddr_in local_addr,client_addr;
static int sin_size;
int zrtcp_srv_open(in_addr_t ip,int port,struct sockaddr_in *local_addr)
{
	int sockfd = 0;
	if(-1 == (sockfd = socket(PF_INET,SOCK_STREAM,0))){
		perror("socket error!");
		exit(1);
	}
	
	local_addr->sin_family = PF_INET;
	local_addr->sin_port = htons(port);
	local_addr->sin_addr.s_addr = INADDR_ANY;
	bzero(&(local_addr->sin_zero),0);

	if(-1 == bind(sockfd,(struct sockaddr *)local_addr,sizeof(struct sockaddr))){
		perror("bind error!");
		exit(1);
	}

	if(-1 == listen(sockfd,LISTEN_LIMIT)){
		perror("listen error");
		exit(1);
	}
	return sockfd;
}

int zrtcp_srv_accept(int sockfd,struct sockaddr_in *client_addr)
{
	int clientfd = 0;
	sin_size = sizeof(struct sockaddr_in);

	if(-1 == (clientfd = accept(sockfd,(struct sockaddr *)client_addr,&sin_size))){
		perror("accept error!");
		exit(1);
	}
	return clientfd;
}

int zrtcp_clt_open(in_addr_t ip,int port,struct sockaddr_in *tag_addr)
{
	int sockfd = 0;
	tag_addr->sin_family = AF_INET;
	tag_addr->sin_port = htons(port);
	tag_addr->sin_addr.s_addr = ip;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(sockfd < 0){
		printf("socket failed!\n");
		return -1;
	}

	if(0 > connect(sockfd,(struct sockaddr*)tag_addr,sizeof(struct sockaddr_in))){
		printf("connect failed!\n");
		return -1;
	}

	return sockfd;
}

