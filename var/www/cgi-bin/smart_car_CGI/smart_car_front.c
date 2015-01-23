#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "smart_car_move.h"



int main(void)
{
    //int fd;
	char *buf="front!";
    //fd = open("test.txt",O_CREAT,0);
//	write(fd,buf,7);
    //printf("front!\n"); 
    smart_car_set(0, 50); //speed=30 Angel=0
  //  close(fd);
    return 0;
}
