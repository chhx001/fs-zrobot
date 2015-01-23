#ifndef IIC_H_INCLUDED
#define IIC_H_INCLUDED

/****************** Include Files ********************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>



#define	EEPROM_ADDR 0x50//IICÐ´ÈëÊ±µÄµØÖ·×Ö½ÚÊý¾Ý£¬+1Îª¶ÁÈ¡

unsigned char eeprom_read(unsigned char REG_Address);
void eeprom_write(unsigned char REG_Address,unsigned char REG_data);

int iic_io_init();

#endif // _MY_IIC_H

