#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "serial.h"
#include "zrcar.h"

#define TEST(x,msg) if(!(x)){ printf(msg); return -1;}
#define CONSTRAIN(x,min,max) (x)=((x)<(min))?(min):((x)>(max))?(max):(x); 

static ZRCarDev zrcar_dev;

int zrcar_wheel_init(char *wheel_dev)
{
	zrcar_dev.wheel_dev = wheel_dev;
	return 0;
}
int zrcar_opt_init(char *optical_dev)
{
	zrcar_dev.optical_dev = optical_dev;
	return 0;
}
int zrcar_ultrasonic_init(char *ultrasonic_dev)
{
	zrcar_dev.ultrasonic_dev = ultrasonic_dev;
	return 0;
}


int zrcar_wheel_l_set(int speed)
{
	int fd = open(zrcar_dev.wheel_dev,O_RDWR);
	TEST(fd > 0,"wheel device open failed!\n");
	CONSTRAIN(speed,-45,45);
	ioctl(fd,WHEEL_L_SET,speed);
	close(fd);
	return 0;
}

int zrcar_wheel_r_set(int speed)
{
	int fd = open(zrcar_dev.wheel_dev,O_RDWR);
	TEST(fd > 0,"wheel device open failed!\n");
	CONSTRAIN(speed,-45,45);
	ioctl(fd,WHEEL_R_SET,speed);
	close(fd);
	return 0;
}

int zrcar_wheel_l_get(int *speed)
{
	int fd = open(zrcar_dev.wheel_dev,O_RDWR);
	TEST(fd > 0,"wheel device open failed!\n");
	TEST(speed != NULL,"Argument pointer is NULL!\n");
	ioctl(fd,WHEEL_L_GET,speed);
	close(fd);
	return 0;
}
	
int zrcar_wheel_r_get(int *speed)
{
	int fd = open(zrcar_dev.wheel_dev,O_RDWR);
	TEST(fd > 0,"wheel device open failed!\n");
	TEST(speed != NULL,"Argument pointer is NULL!\n");
	ioctl(fd,WHEEL_R_GET,speed);
	close(fd);
	return 0;
}

int zrcar_opt_get_all(int *val)
{
	int fd = open(zrcar_dev.optical_dev,O_RDWR);
	TEST(fd > 0,"optical device open failed!\n");
	TEST(val != NULL,"Argument pointer is NULL!\n");
	ioctl(fd,OPTICAL_GET_ALL,val);
	close(fd);
	return 0;
}
int zrcar_opt_get_sg(int *val,int num)
{
	int fd = open(zrcar_dev.optical_dev,O_RDWR);
	TEST(fd > 0,"optical device open failed!\n");
	TEST(val != NULL,"Argument pointer is NULL!\n");
	ioctl(fd,OPTICAL_SET_SG_MASK,num);
	ioctl(fd,OPTICAL_GET_SG,val);
	close(fd);
	return 0;
}

int zrcar_ultra_get_all(float val_arr[])
{
	unsigned int temp[3] = {0};
	int fd = open(zrcar_dev.ultrasonic_dev,O_RDWR);
	TEST(fd > 0,"ultrasonic device open failed!\n");
	TEST(val_arr != NULL,"Argument pointer is NULL!\n");
	ioctl(fd,ULTRASONIC_ECHO_GET,temp);
	int i = 0;
	for(i = 0;i < 3; ++ i){
		val_arr[i] = temp[i] * 0.0017;
	}
	close(fd);
	return 0;
}

