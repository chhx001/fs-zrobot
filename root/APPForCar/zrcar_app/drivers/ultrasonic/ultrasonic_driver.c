/**********************************************************************
*@Filename          ultrasonic_driver.c
*@Author            ZRobot
*@Des
*@History
*                Original version, 16-Jul, 2013 zulongp
*                modify for zrobot IV,  29-Aug, 2014
*				 modify for ZRobot , 	26-Dec, 2014
**********************************************************************/
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/ioctl.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/wait.h>
#include <linux/cdev.h>

#include <linux/interrupt.h>
#include <asm/signal.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/semaphore.h>

#include "ultrasonic_cmd.h"


MODULE_AUTHOR("ZRobot");
MODULE_DESCRIPTION("Ultrasonic driver of ZRobot car");
MODULE_VERSION("V1.0");
MODULE_LICENSE("GPL");

#define DEVICE_NAME "zrcar_ultrasonic_dev"
#define ULTRASONIC_BASEADDR 0x43C10000
#define INTERRUPT_ID 91
#define IRQ_MASK 0x1
#define IRQ_DEVICE_ID 0

#define ULTRASONIC_STATUS_OFFSET 0
#define ULTRASONIC_ECHO1_OFFSET  0x04
#define ULTRASONIC_ECHO2_OFFSET  0x08
#define ULTRASONIC_ECHO3_OFFSET  0x0C

/*#define MAGIC 'u'


#define ULTRASONIC_ECHO_GET	_IOR(MAGIC,1,int)
*/
#define ULTRASONIC_IOC_MAXNR     10

static int ultrasonic_major = 0;
static unsigned long ultrasonic_baseaddr = 0;
static struct class* ultrasonic_class = NULL;
static struct device* ultrasonic_dev = NULL;

typedef struct {
    dev_t dev_no;
    u32 IsReady;
    int InterruptPresent;
} UltraIntr;

static UltraIntr ultra_intr_dev;
static struct semaphore sem;

static irqreturn_t ultrasonic_intr(int irq, void *dev_id, struct pt_regs *regs)
{
	printk("ultra_intr!\n");
//	up(&sem);
	return 0;
}


static ssize_t ultrasonic_open(struct inode *inode, struct file *file)
{
	int ret = 0;

	sema_init(&sem,0);
	printk("open success\n");
	return 0;
}

static ssize_t ultrasonic_release(struct inode *inode, struct file *file)
{
		return 0;
}

static int get_ultra_echo(u32 status[],int len)
{
	iowrite32(0x1,ultrasonic_baseaddr + ULTRASONIC_STATUS_OFFSET);
	//down(&sem);
	int i = 0;
	unsigned int cur = ULTRASONIC_ECHO1_OFFSET;
	unsigned int end = ULTRASONIC_ECHO3_OFFSET;
	unsigned int step = 0x04;
	do{
		while(!(ioread32(ultrasonic_baseaddr) & (1 << (i+1))));
		status[i] = ioread32(ultrasonic_baseaddr + cur);
		printk("echo status[%d]=%d\n",i,status[i]);
		cur += step;
		++ i;
	}while(cur <= end && i < len);
	return 0;
}



//static ssize_t ultrasonic_ioctl(struct inode *inode, struct file *file, 
//arg is a baseaddress of a long array which contains three elements.
static ssize_t ultrasonic_ioctl(struct file *file, 
								unsigned int cmd,
								unsigned long arg)
{
	u32 status[3] = {1,2,3};
	int ret = 0;
	switch(cmd)
	{
	case ULTRASONIC_ECHO_GET:
		ret = get_ultra_echo(status,3);
		ret = __put_user(status[0], (u32 *)arg);
		ret = __put_user(status[1], (u32 *)arg+1);
		ret = __put_user(status[2], (u32 *)arg+2);
		break;
	default:
		printk("cmd=%d\n",cmd);
		break;
	}
	return 0;
}

static struct file_operations ultrasonic_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = ultrasonic_ioctl,
	.open = ultrasonic_open,
	.release = ultrasonic_release,
};
static int __init ultrasonic_init(void) 
{
	int ret;
	ret = register_chrdev(0,DEVICE_NAME, &ultrasonic_fops);
	if(ret < 0)
	{
		printk("ultrasonic: can't get major number\n");
		return ret;
	}
	ultrasonic_major = ret;
	ultrasonic_class = class_create(THIS_MODULE, "ultrasonic_class");
	if(IS_ERR(ultrasonic_class))
	{
		printk("ultrasonic: failed in creating class\n");
		unregister_chrdev(ultrasonic_major, DEVICE_NAME);
		return -1;
	}
	ultrasonic_dev = device_create(ultrasonic_class,\
									  NULL, \
									  MKDEV(ultrasonic_major, 0), \
									  NULL,\
									  DEVICE_NAME);
	if(IS_ERR(ultrasonic_dev))
	{
		printk("ultrasonic: failed in creating device!\n");
		unregister_chrdev(ultrasonic_major, DEVICE_NAME);
		class_unregister(ultrasonic_class);
		class_destroy(ultrasonic_class);
		return -1;
	}
	ultrasonic_baseaddr = (unsigned long) ioremap(ULTRASONIC_BASEADDR, sizeof(u32));
	/*ret = request_irq(INTERRUPT_ID, ultrasonic_intr,IRQF_TRIGGER_RISING, DEVICE_NAME, &ultra_intr_dev);
	if(ret)
    {
        printk("could not register interrupt!");
        return ret;
    }*/

	printk("Ultrasonic initial successfully\n");
	return 0;
}
static void __exit ultrasonic_exit(void)
{
	//free_irq(INTERRUPT_ID,&ultra_intr_dev);
  	device_destroy(ultrasonic_class, MKDEV(ultrasonic_major, 0));
  	class_unregister(ultrasonic_class);
  	class_destroy(ultrasonic_class);
  	unregister_chrdev(ultrasonic_major, DEVICE_NAME);
  	printk("ultrasonic module exit.\n");
}
module_init(ultrasonic_init);
module_exit(ultrasonic_exit);
