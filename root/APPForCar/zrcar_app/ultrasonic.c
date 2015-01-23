#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "zrcar.h"



void car_stop()
{
	zrcar_wheel_l_set(0);
	zrcar_wheel_r_set(0);
}
void car_forward()
{
	zrcar_wheel_l_set(-8);
	zrcar_wheel_r_set(-8);
}
void car_back()
{
	zrcar_wheel_l_set(8);
	zrcar_wheel_r_set(8);
}
void car_right()
{
	zrcar_wheel_l_set(0);
	zrcar_wheel_r_set(-8);
}
void car_left()
{
	zrcar_wheel_l_set(-8);
	zrcar_wheel_r_set(0);
}
int main()
{
	/*********init*******/
	zrcar_wheel_init(ZRCAR_WHEEL_DEV);
	zrcar_ultrasonic_init(ZRCAR_ULTRASONIC_DEV);

	// Ultrasonic__value_all
	float ult_data[3];

	 car_forward();

	 int i;
	while(1)
	{
		/***************optical read**************/
		zrcar_ultra_get_all(ult_data);
		for(i = 1; i < 4; i++){
			printf("u%d : %f mm",i , ult_data[i-1]);
		}
		printf("\n");
		if((ult_data[2] >0 &&ult_data[2] < 180) || (ult_data[1] >0 &&ult_data[1] < 180) ){
			car_back();
			usleep(500000);    //delay
			car_left();
			usleep(800000);    //delay
		}
		else if( ult_data[0] >0&&ult_data[0] < 180 ){
			car_back();
			usleep(500000);    //delay
			car_right();
			usleep(800000);    //delay
		}
		else {
			car_forward();
		}
		usleep(200000);    //delay
	}

    return 0;
}

