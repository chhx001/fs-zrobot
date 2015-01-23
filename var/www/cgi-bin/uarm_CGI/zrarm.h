#ifndef ZRARM_H_INCLUDED
#define ZRARM_H_INCLUDED

#include "uarm_cmd.h"
#include "uarm.h"
#include "iic.h"

#define ZRARM_UARM_DEV "/dev/zrarm_uarm_dev"
#define ZRARM_STATE_FILE "/tmp/uarm.info"


int zrarm_init();
int zrarm_save_state(char *);
int zrarm_restore_state(char *);
int zrarm_set_pos(short stretch,short height,short arm_rot,short hand_rot);
int zrarm_servo_write(char channel,short value);
int zrarm_catch();
int zrarm_release();
int zrarm_limit_state();
int zrarm_move(short x,short y,short z,short hand_rot);

int zrarm_btn_state(int num);
int zrarm_get_state(int *s,int *h,int *r,int *hand);

int zrarm_ad_read(unsigned char channel);
int zrarm_eeprom_read(unsigned char channel);
void zrarm_eeprom_write(unsigned char channel,unsigned char data);

//void zrarm_calibration();

#endif
