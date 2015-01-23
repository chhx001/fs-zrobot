#include <stdio.h>
#include "zrarm.h"

int main()
{
	zrarm_init();
	int t;
	t = zrarm_eeprom_read(1);
	printf("t:%d\n",t);
	while(1){
		sleep(1);
		zrarm_set_pos(100,50,-10,0);
		sleep(1);
		zrarm_set_pos(100,50,10,0);
	}

	return 0;
}
