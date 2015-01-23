#include <stdio.h>
#include <stdlib.h>

#include "zrtcp.h"
#include <math.h>
#include <unistd.h>
#include "zrcar.h"
void constrain_and_run(short, short); 
int main(int argc,char **argv)
{
	int sockfd,datafd;
	struct sockaddr_in server_addr,client_addr;
	short speed_value;
	short angle_value;
	short control_value;
	short length_value;
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
	//	printf("received:%d,control_segamentation:%d \n datalength:%d anglevalue:%d \n speed_value:%d \n",received,*((short *)buf),*((short *)(buf+2)),*((short*)(buf+4)),*((short*)(buf+6)));
	speed_value = *((short*)(buf+6));
	angle_value = *((short*)(buf+4));
	control_value = *((short*)buf);
	length_value=*((short*)(buf+2));
//	printf("Speed Value: %d\n\n",speed_value);
//	printf("Angle Value: %d\n\n",angle_value);
	if(control_value==1792&&length_value==1024){
	constrain_and_run(speed_value, angle_value);}
	}

	close(datafd);
	close(sockfd);
		
	return 0;
}
void constrain_and_run(short speed, short angle)
{
	short left_speed;
	short right_speed;
	if(speed==0)
	{
		printf("The Value is Zero!!!\n");
		printf("The Value is Zero!!!\n");
		printf("The Value is Zero!!!\n");
	}
	if(speed<0)
	{
		if(angle < 0){
			if(speed <= -45){
				right_speed=-45;
				left_speed=right_speed*cos(angle);
			}
			else{
				right_speed=speed;
				left_speed=right_speed*cos(angle);
			}
		}
		else{
			
			if(speed <= -45){
				left_speed=-45;
				right_speed=left_speed*cos(angle);
			}
			else{
				left_speed=speed;
				right_speed=left_speed*cos(angle);
			}
		}

	}
	else{
		
		if(angle < 0){
			if(speed >= 45){
				right_speed=45;
				left_speed=right_speed*cos(angle);
			}
			else{
				right_speed=speed;
				left_speed=right_speed*cos(angle);
			}
		}
		else{
			
			if(speed >= 45){
				left_speed=45;
				right_speed=left_speed*cos(angle);
			}
			else{
				left_speed=speed;
				right_speed=left_speed*cos(angle);
			}
		}
	}
	printf("left speed:%d \nright_speed:%d \n",left_speed, right_speed);
	zrcar_wheel_init(ZRCAR_WHEEL_DEV);
	zrcar_wheel_l_set(left_speed);
	zrcar_wheel_r_set(right_speed);
}
