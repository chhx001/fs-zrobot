#ifndef GPIO_CMD_H_INCLUDED
#define GPIO_CMD_H_INCLUDED

#define GPIO_MAGIC 'g'
#define GPIO_UP _IOW(GPIO_MAGIC,1,int)
#define GPIO_SET _IOW(GPIO_MAGIC,4,int)
#define GPIO_DOWN _IOW(GPIO_MAGIC,2,int)
#define GPIO_GET _IOR(GPIO_MAGIC,3,int)


#endif
