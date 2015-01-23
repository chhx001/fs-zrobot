#include <stdio.h>
#include <stdlib.h>
#include "zrtcp.h"

int main(int argc,char *argv[])
{
	struct sockaddr_in tag_addr;
	int sockfd = 0;
	char buf[64] = {1,2,3,4};
	if(-1 == (sockfd = zrtcp_clt_open(ZR_IP("173.17.1.143"),8800,&tag_addr))){
		printf("socket open failed");
		exit(1);
	}
	
	//while(1){
		if(-1 == send(sockfd,buf,4,0)){
			perror("send error");
			close(sockfd);
		//	break;
		}
		//sleep(2);
	//}
	close(sockfd);
	return 0;
}
