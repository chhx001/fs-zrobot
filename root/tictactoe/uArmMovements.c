#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "zrarm.h"

#define TIMETIP 50000
#define WAIT usleep(100000);

int lx = -20,ly = 130,lz = 0;

void armmove(int x,int y,int z)
{
	double tx = lx,ty = ly,tz = lz;
	int count = 0;
	do{
		zrarm_move(tx,ty,tz,0);
		tx += (x - lx) / (1000000.0 / TIMETIP);
		ty += (y - ly) / (1000000.0 / TIMETIP);
		tz += (z - lz) / (1000000.0 / TIMETIP);
		printf("move to:(%lf,%lf)\n",tx,ty);
		usleep(TIMETIP);
		count ++;
	}while(count <= 1000000.0 / TIMETIP); 
	lx = x;
	ly = y;
	lz = z;
}

void armcatch()
{
	int s,h,r,hand,th;
	zrarm_get_state(&s,&h,&r,&hand);
	th = h;
	while(th > -180 && !zrarm_limit_state()){
		th -= 3;
		zrarm_set_pos(s,th,r,hand);
		usleep(TIMETIP);
	}
	zrarm_catch();
	WAIT;
	while(th < h){
		th += 3;
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
		th -= 3;
		zrarm_set_pos(s,th,r,hand);
		usleep(TIMETIP);
	}
	zrarm_release();
	WAIT;
	while(th < h){
		th += 3;
		zrarm_set_pos(s,th,r,hand);
		usleep(TIMETIP);
	}
}


int uArmCtrl(int x,int y,int z,int cmd)
{
	int logx = lx,logy = ly,logz = lz;
	char *data;
	char isdragged = 0;
	switch(cmd){
	case 1:
		//zrarm_move(x,y,z,0);
		armmove(x,y,z);
		break;
	case 18:
		//zrarm_move(x,y,z,0);
		armmove(x,y,z);
		armcatch();
		armmove(logx,logy,logz);
		//zrarm_set_pos(s,h,r,hr);
		break;
	case 19:
		//zrarm_move(x,y,z,0);
		armmove(x,y,z);
		release();
		armmove(logx,logy,logz);
		//zrarm_set_pos(s,h,r,hr);
		break;
	default:
		break;
	}
    return 0;
}
