#ifndef ZRCAR_H_INCLUDED
#define ZRCAR_H_INCLUDED

#include "wheel_cmd.h"
#include "ultrasonic_cmd.h"
#include "optical_cmd.h"

//默认设备名
#define ZRCAR_WHEEL_DEV		"/dev/zrcar_wheel_dev"
#define ZRCAR_OPTICAL_DEV		"/dev/zrcar_optical_dev"
#define ZRCAR_ULTRASONIC_DEV	"/dev/zrcar_ultrasonic_dev"

/*****************************************************************************/
/*	设备驱动的地址集合，该结构体应以static形式存在于.c文件
*/
typedef struct{
	char *wheel_dev;						//轮子
	char *optical_dev;						//光电
	char *ultrasonic_dev;					//超声波
} ZRCarDev;

/*****************************************************************************/
/*
**以各个设备名初始化小车设备
**	zrcar_XXX_init(
**		XXX_dev:XXX的设备路径，一般在/dev目录下
**	）
**	return:一个整形数，显示是否初始化成功，若成功则返回0
*/
int zrcar_wheel_init(char *wheel_dev);
int zrcar_opt_init(char *optical_dev);
int zrcar_ultrasonic_init(char *ultrasonic_dev);

/*****************************************************************************/
/*
**左/右车轮速度设置
**	zrcar_wheel_l_set/zrcar_wheel_r_set(
**		speed:速度，范围-45~45
**	）
**	return:一个整形数，速度设置是否成功，成功为0
*/
int zrcar_wheel_l_set(int speed);
int zrcar_wheel_r_set(int speed);
/*
**左/右车轮速度获取
**	zrcar_wheel_l_get/zrcar_wheel_r_get(
**		*speed:整形指针，获取的速度放在这里
**	）
**	return:一个整形数，速度获取是否成功，成功为0
*/
int zrcar_wheel_l_get(int *speed);
int zrcar_wheel_r_get(int *speed);
/*****************************************************************************/
/*
**光电数值获取（全部）
**	zrcar_opt_get_all(
**		*val:整形指针，获取的值放在这里，从低到高按位置位
**	）
**	return:一个整形数，获取是否成功，成功为0
*/
int zrcar_opt_get_all(int *val);
/*
**光电数值获取（单个）
**	zrcar_opt_get_sg(
**		*val:整形指针，获取的值放在这里
**		num:目标光电传感器的编号
**	）
**	return:一个整形数，获取是否成功，成功为0
*/
int zrcar_opt_get_sg(int *val,int num);
/*****************************************************************************/
/*
**超声波数值获取（全部）
**	zrcar_ultra_get_all(
**		val_arr[]:整形数组，获取的值放在这里,总共有0,1,2三个
**	）
**	return:一个整形数，获取是否成功，成功为0
*/
int zrcar_ultra_get_all(float val_arr[]);
//单个获取超声波数值，尚未实装
//int zrcar_ultrasonic_get_sg(int *val,int num);
/*****************************************************************************/

	
#endif
	
