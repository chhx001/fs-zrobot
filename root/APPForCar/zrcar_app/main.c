#include <stdio.h>
#include <unistd.h>
#include "zrcar.h"

int main()
{
/*	zrcar_wheel_init(ZRCAR_WHEEL_DEV);
	zrcar_wheel_l_set(0);
	zrcar_wheel_r_set(0);
	int getl,getr;
	zrcar_wheel_l_get(&getl);
	zrcar_wheel_r_get(&getr);
	printf("l=%d,r=%d",getl,getr);*/

	/*zrcar_opt_init(ZRCAR_OPTICAL_DEV);
	int val;
	while(1){
		zrcar_opt_get_sg(&val,1);
		printf("val:%x\n",val);
		sleep(1);
	}*/
	float test[3] = {1,2,3};
	zrcar_ultrasonic_init(ZRCAR_ULTRASONIC_DEV);
	while(1){
		zrcar_ultra_get_all(test);
		int i = 0;
		for(i = 0;i < 3; ++ i){
			printf("test[%d]=%f\n",i,test[i]);
		}
		sleep(1);
	}	
	return 0;
}
