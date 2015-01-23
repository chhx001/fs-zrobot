#include <stdio.h>
#include <unistd.h>
#include "zrcar.h"

int main(int argc, char** argv)
{
	zrcar_wheel_init(ZRCAR_WHEEL_DEV);
	zrcar_wheel_l_set(0);
	zrcar_wheel_r_set(0);
	return 0;
}
