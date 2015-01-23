#include "iic.h"
#include "zrarm.h"
#include "uarm_cmd.h"
#include <unistd.h>
#include <stdlib.h>

static int tfd = 0;

#define CONSTRAIN(x,min,max) (x)=((x)>(max))?(max):((x)<(min))?(min):(x);

//**************************************
//ÏòI2CÉè±¸Ð´ÈëÒ»¸ö×Ö½ÚÊý¾Ý
//**************************************
void Single_WriteI2C(unsigned char REG_Address,unsigned char REG_data)
{
//	CONSTRAIN(REG_Address,0,3);
    int retn = 0;
	tfd=open("/sys/bus/i2c/devices/0-0050/eeprom",O_RDWR); 
	if(tfd < 0){
		printf("eeprom open failed!\n");
		exit(1);
	}
	lseek(tfd,REG_Address,SEEK_SET);
	write(tfd,&REG_data,1);
	close(tfd);

}
//**************************************
//´ÓI2CÉè±¸¶ÁÈ¡Ò»¸ö×Ö½ÚÊý¾Ý 
//**************************************
unsigned char Single_ReadI2C(unsigned char REG_Address)
{
	unsigned char REG_data;
	tfd=open("/sys/bus/i2c/devices/0-0050/eeprom",O_RDWR); 
	if(tfd < 0){
		printf("eeprom open failed!\n");
		exit(1);
	}
	lseek(tfd,REG_Address,SEEK_SET);
	read(tfd,&REG_data,1);
	close(tfd);
	return REG_data;
}
unsigned char eeprom_read(unsigned char REG_Address)
{
	return Single_ReadI2C(REG_Address);
}

void eeprom_write(unsigned char REG_Address,unsigned char REG_data)
{
	Single_WriteI2C( REG_Address, REG_data);
}
//void print(char *str);
int iic_io_init()
{

    return 1;
}



