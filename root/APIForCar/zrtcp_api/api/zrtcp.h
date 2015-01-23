#ifndef ZRTCP_H_INCLUDED
#define ZRTCP_H_INCLUDED

//#include <stdio.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/wait.h>
//#include <string.h>

#define ZR_IP(tag_ip) inet_addr(tag_ip)

int zrtcp_srv_open(in_addr_t ip,int port,struct sockaddr_in *local_addr);
int zrtcp_srv_accept(int sockfd,struct sockaddr_in *client_addr);

int zrtcp_clt_open(in_addr_t ip,int port,struct sockaddr_in *tag_addr);


#endif
