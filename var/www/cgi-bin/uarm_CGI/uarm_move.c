#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "zrarm.h"

#define TIMETIP 30000

void catch()
{
	int s,h,r,hand,th;
	zrarm_get_state(&s,&h,&r,&hand);
	th = h;
	while(th > -180 && !zrarm_limit_state()){
		th -= 5;
		zrarm_set_pos(s,th,r,hand);
		usleep(TIMETIP);
	}
	zrarm_catch();
	while(th < h){
		th += 5;
		zrarm_set_pos(s,th,r,hand);
		usleep(TIMETIP);
	}
}

void release()
{
	int s,h,r,hand,th;
	zrarm_get_state(&s,&h,&r,&hand);
	th = h;
	while(th > -180 && !zrarm_limit_state()){
		th -= 5;
		zrarm_set_pos(s,th,r,hand);
		usleep(TIMETIP);
	}
	zrarm_release();
	while(th < h){
		th += 5;
		zrarm_set_pos(s,th,r,hand);
		usleep(TIMETIP);
	}
}


int main(void)
{
    int fd;
    int x,y,z,cmd;
	int height=50;
	char *data;
	char isdragged = 0;
	data = getenv("QUERY_STRING");
	sscanf(data,"x=%d&y=%d&cmd=%d",&x,&y,&cmd);
	if(zrarm_restore_state(ZRARM_STATE_FILE) < 0){
		if(access(ZRARM_STATE_FILE,F_OK) == -1)
			zrarm_init();
		else
			return 0;
	}
	switch(cmd){
	case 1:
		zrarm_move(x,y,height,0);
		break;
	case 18:
		catch();
		break;
	case 19:
		release();
		break;
	default:
		break;
	}
	zrarm_save_state(ZRARM_STATE_FILE);
    return 0;
}
