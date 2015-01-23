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
	zrcar_opt_init(ZRCAR_OPTICAL_DEV);


	// Optical__value_all
	int optical_data;

	 car_forward();


	while(1)
	{
		/***************optical read**************/
		zrcar_opt_get_all(&optical_data);
				if((optical_data)==0)
				{
					car_stop();
				}
				else if((optical_data) & 0x01){                           //turn right
			    	car_right();
			    }
			    else if((optical_data) & 0x10){                       //turn left
			    	car_left();
			    }
			    else {
			    	car_forward();
			    }
			    usleep(200000);    //delay 0.2s
	}

    return 0;
}


