#include <stdio.h>
#include <stdlib.h>

#include "zrtcp.h"

int main(int argc,char **argv)
{
	int sockfd,datafd;
	struct sockaddr_in server_addr,client_addr;

	sockfd = zrtcp_srv_open(INADDR_ANY,8800,&server_addr);
	datafd = zrtcp_srv_accept(sockfd,&client_addr);
	int received = 0;
	char buf[64] = {0};
	while(1){
		received = recv(datafd,buf,64,0);

		if(-1 == received){
			perror("recv error");
			break;
		}
		else if(0 == received){
			printf("tcp disconnected");
			break;
		}
		printf("received:%d,buf[0-3]:%x\n",received,*((int *)buf));
	}

	close(datafd);
	close(sockfd);
		
	return 0;
}


