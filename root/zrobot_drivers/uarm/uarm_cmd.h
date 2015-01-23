#ifndef UARM_CMD_H_INCLUDED
#define UARM_CMD_H_INCLUDED

#define UARM_MAGIC 'a'
#define UARM_INIT _IOW(UARM_MAGIC,1,int)
#define UARM_REG_WRITE _IOW(UARM_MAGIC,2,int)
#define UARM_AD_READ _IOR(UARM_MAGIC,3,int)

//#define UARM_IIC_INIT _IOW(UARM_MAGIC,4,int)
//#define UARM_IIC_WRITE _IOW(UARM_MAGIC,5,int)
//#define UARM_IIC_READ _IOR(UARM_MAGIC,6,int)

#define UARM_DETACH _IOW(UARM_MAGIC,7,int)
#define UARM_ATTACH _IOW(UARM_MAGIC,8,int)

#define UARM_REG_READ _IOR(UARM_MAGIC,9,int)

#define UARM_BTN_READ _IOW(UARM_MAGIC,12,int)

struct ZRUarmReg{
	unsigned char channel;
	short value;
};

struct ZRADAccess{
	unsigned char channel;
	int value;
};

struct ZRIICAccess{
	unsigned char addr;
	char data;
};

#endif
