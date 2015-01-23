#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

extern int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);
extern int open_port(int fd,char *dev);
extern int open_serial(char *dev,int band);


#endif // SERIAL_H_INCLUDED
