#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "zrarm.h"
#include "uarm.h"

static UArm uarm;


int zrarm_init()
{
	uarm = getUArm();
}

int zrarm_servo_write(char channel,short value)
{
	int fd=open(ZRARM_UARM_DEV,O_RDWR);
	struct ZRUarmReg reg;
	reg.channel = channel;
	reg.value = value;
	ioctl(fd,UARM_REG_WRITE,&reg);
	close(fd);
	return 0;
}

int zrarm_set_pos(short s,short h,short ar,short hr)
{
	uarm.setSHR(&uarm,s,h,ar,hr);
	return 0;
}

int zrarm_catch()
{
	uarm.pumpOn(&uarm);
	return 0;
}

int zrarm_release()
{
	uarm.pumpOff(&uarm);
	return 0;
}

int zrarm_limit_state()
{
	return uarm.ifSwitchOn(&uarm);
}

int zrarm_move(short x,short y,short z,short hand_rot)
{
	uarm.setXYZ(&uarm,x,y,z,hand_rot);
	return 0;
}

int zrarm_save_state(char *state)
{
	int fd=open(state,O_RDWR|O_CREAT);
	write(fd,&uarm,sizeof(UArm));
	close(fd);
	return 0;
}

int zrarm_restore_state(char *state)
{
	int fd=open(state,O_RDWR);
	if(fd < 0){
		//printf("state file doesn't exist!\n");
		return -1;
	}
	read(fd,&uarm,sizeof(UArm));
	close(fd);
}

int zrarm_btn_state(int num)
{
	int fd=open(ZRARM_UARM_DEV,O_RDWR);
	struct ZRUarmReg reg;
	reg.channel = num;
	ioctl(fd,UARM_BTN_READ,&reg);
	close(fd);
	return (int)reg.value;
}

static void attach()
{
	int fd=open(ZRARM_UARM_DEV,O_RDWR);
	ioctl(fd,UARM_ATTACH,0);
	close(fd);
}

static void detach()
{
	int fd=open(ZRARM_UARM_DEV,O_RDWR);
	ioctl(fd,UARM_DETACH,0);
	close(fd);
}

static int transform_angle(int channel,int angle)
{
	unsigned int pwm_duty = uarm.attr.servo[channel].pwm_min + (uarm.attr.servo[channel].pwm_max - uarm.attr.servo[channel].pwm_min)/180 * angle;
	return pwm_duty;
}


/*void zrarm_calibration()
{
	int initPosL = INIT_POS_L + 20; // Added 20 degrees here to start at reasonable point
    int initPosR = INIT_POS_R + 20; // Added 20 degrees here to start at reasonable point

    signed char offset_l = 0;
    signed char offset_r = 0;

	if(zrarm_btn_state(1))
	{
		sleep(2);// wait for 2 sec
		if(zrarm_btn_state(1)) //goto calibration
		{

			detach();
			printf("calibration: detach ...\r\n");
			while(zrarm_btn_state(1));
			usleep(500000);

			while(1)
			{
				// SG-> Commentary: While user adjusts for minimum angles, keep track of angle and add
				//                  margin of analog reading value of 12, which is about 3 degrees.
				int minAngle_L = zrarm_ad_read(SERVO_L) + 15;
				int minAngle_R = zrarm_ad_read(SERVO_R) + 15;

				if(zrarm_btn_state(1))
				{


					printf("Start calibration ...\r\n");
					usleep(500000);  //SG-> Added to delay for user to remove hand

					zrarm_servo_write(SERVO_R, transform_angle(SERVO_R,initPosR));
					zrarm_servo_write(SERVO_L, transform_angle(SERVO_L,initPosL));
					attach();
					usleep(500000);

					while(zrarm_ad_read(SERVO_R) < minAngle_R - SAMPLING_DEADZONE)
					{
						zrarm_servo_write(SERVO_R, transform_angle(SERVO_R,++initPosR));
						usleep(50000);
					}

					while(zrarm_ad_read(SERVO_R) > minAngle_R + SAMPLING_DEADZONE)
					{
						zrarm_servo_write( SERVO_R, transform_angle(SERVO_R,--initPosR));
						usleep(50000);
					}

					while(zrarm_ad_read(SERVO_L) < minAngle_L - SAMPLING_DEADZONE)
					{
						zrarm_servo_write( SERVO_L, transform_angle(SERVO_L,++initPosL));
						usleep(50000);
					}

					while(zrarm_ad_read(SERVO_L) > minAngle_L + SAMPLING_DEADZONE)
					{
						zrarm_servo_write( SERVO_L, transform_angle(SERVO_L,--initPosL));
						usleep(50000);
					}
					offset_l = initPosL - INIT_POS_L;
					offset_r = initPosR - INIT_POS_R;

					zrarm_eeprom_write(0, CALIBRATION_FLAG);  // Added flag to know if offset is done
					usleep(10000);
					zrarm_eeprom_write(1, offset_l);			// offsetL
					usleep(10000);
					zrarm_eeprom_write(2, offset_r);			// offsetR
					usleep(10000);
					// buzzer alert

					// reset postion
					uarm.attr.offsetL = zrarm_eeprom_read(1);
    				uarm.attr.offsetR = zrarm_eeprom_read(2);
					break;
				}
			}
        }

	}


}*/

int zrarm_get_state(int *s,int *h,int *r,int *hand)
{
	*s=(int)uarm.status.pos.s;
	*h=(int)uarm.status.pos.h;
	*r=(int)uarm.status.pos.r;
	*hand=(int)uarm.status.pos.handrot;
}


int zrarm_ad_read(unsigned char channel)
{
	struct ZRADAccess ad_access;
	int i,sum = 0;
	ad_access.channel = channel;
	int fd=open(ZRARM_UARM_DEV,O_RDWR);
	if(fd < 0){
		printf("zrarm dev open error");
		exit(1);
	}
	for(i=0;i < 5; ++ i){
		ioctl(fd,UARM_AD_READ,&ad_access);
		sum += ad_access.value;
	}
	close(fd);
	return sum / 5;
}

int zrarm_eeprom_read(unsigned char channel)
{
	return eeprom_read(channel);
}

void zrarm_eeprom_write(unsigned char channel,unsigned char data)
{
	eeprom_write(channel,data);
}
