/**********************************************************************
*@Filename          optical.c
*@Author            ZRobot
*@Des
*@History
*                Original version, 29-Aug, 2014 
*				 Modified for ZRobot 26-Dec,2014
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

#include "optical_cmd.h"


MODULE_AUTHOR("ZRobot");
MODULE_DESCRIPTION("Optical driver of ZRobot car");
MODULE_VERSION("V1.0");
MODULE_LICENSE("GPL");

#define DEVICE_NAME "zrcar_optical_dev"
#define OPTICAL_BASEADDR 0x41210000
#define INTERRUPT_ID 90
#define IRQ_MASK 0x1
#define IRQ_DEVICE_ID 0
#define XGPIO_GIE_OFFSET    0x11C /**< Glogal interrupt enable register */
#define XGPIO_ISR_OFFSET    0x120 /**< Interrupt status register */
#define XGPIO_IER_OFFSET    0x128 /**< Interrupt enable register */
#define XGPIO_GIE_GINTR_ENABLE_MASK 0x80000000

/*#define MAGIC 'o'

#define OPTICAL_GET_ALL		_IOR(MAGIC,1,int)
#define OPTICAL_SET_SG_MASK	_IOW(MAGIC,2,int)
#define OPTICAL_WAIT_READ	_IOR(MAGIC,3,int)
#define OPTICAL_GET_SG		_IOR(MAGIC,4,int)
*/

static int optical_major = 0;
static unsigned long base_addr = 0;
static struct class* optical_class = NULL;
static struct device* optical_dev = NULL;

typedef struct {
    dev_t dev_no;
    u32 IsReady;
    int InterruptPresent;
}OpticalIntr;

static OpticalIntr optical_intr_dev;
static struct semaphore sem;
static int flag;
static u32 optical_readmask = 0;

static void optical_intr_clear()
{
        u32 val;
        val = ioread32(base_addr + XGPIO_ISR_OFFSET);
        iowrite32(val & IRQ_MASK,base_addr + XGPIO_ISR_OFFSET);
        printk("int cleared");
}
static irqreturn_t optical_intr(int irq, void *dev_id, struct pt_regs *regs)
{
	printk("in interrupt\n");
   	up(&sem);
    printk("V\n");
	optical_intr_clear();
    return 0;

}
static void optical_intr_enable()
{
    u32 val = 0;
    //printk("debug1 read addr = %lx\n",key_addr+XGPIO_IER_OFFSET);
    //            0x128
    //val = ioread32(key_addr + XGPIO_IER_OFFSET);
    //printk("debug2 val=%lx\n",val);
    //                   0x128                  0x1
    iowrite32(val | IRQ_MASK,base_addr + XGPIO_IER_OFFSET);
    //printk("debug3 ");
    //           0x11c            0x80000000
    iowrite32(XGPIO_GIE_GINTR_ENABLE_MASK,base_addr + XGPIO_GIE_OFFSET);
}

static ssize_t optical_open(struct inode *inode, struct file *file)
{
	int ret;
	flag = 0;
	sema_init(&sem,0);
    ret = request_irq(INTERRUPT_ID, optical_intr,IRQF_TRIGGER_RISING, DEVICE_NAME, &optical_intr_dev);
    if(ret)
    {
        printk("could not register interrupt!");
        return ret;
    }

	printk("open success\n");
	return 0;
}

static ssize_t optical_release(struct inode *inode, struct file *file)
{
	free_irq(INTERRUPT_ID,&optical_intr_dev);
	return 0;
}

ssize_t optical_read(struct file *filp, char __user *buf, size_t count)
{
	int val,ret;
	val = inb(base_addr);
	ret = __put_user(val,buf);
	return ret;
}

//static ssize_t optical_ioctl(struct inode *inode, struct file *file, 
//arg is a baseaddress of a long array which contains three elements.
static ssize_t optical_ioctl(struct file *file, 
								unsigned int cmd,
								unsigned long arg)
{
	u32 status = 0;
	int ret = 0;
	int *val = 0;
	switch(cmd)
	{
	case OPTICAL_GET_ALL:
		status = ioread32(base_addr);
		ret = __put_user(status, (u32 *)arg);
		break;
	case OPTICAL_SET_SG_MASK:
		optical_readmask = arg;
		break;
	case OPTICAL_GET_SG:
		status = ioread32(base_addr);
		status = (status >> optical_readmask) & 0x1;
		ret = __put_user(status, (u32 *)arg);
		break;
	case OPTICAL_WAIT_READ:
		status = ioread32(base_addr);
		val = (u32 *)arg;
		if(*val==status) 
		{
			//enter wait status
			optical_intr_enable();
			printk("P\n");
			down(&sem);
			status = ioread32(base_addr);
		}
		ret = __put_user(status, (u32 *)arg);
		break;
	default:
		printk("cmd=%d\n",cmd);
		break;
	}
	return 0;
}

static struct file_operations optical_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = optical_ioctl,
	.open = optical_open,
	.release = optical_release,
	.read = optical_read,
};

static int __init optical_init(void) 
{
	int ret;
	ret = register_chrdev(0,DEVICE_NAME, &optical_fops);
	if(ret < 0)
	{
		printk("optical: can't get major number\n");
		return ret;
	}
	optical_major = ret;
	optical_class = class_create(THIS_MODULE, "optical_class");
	if(IS_ERR(optical_class))
	{
		printk("optical: failed in creating class\n");
		unregister_chrdev(optical_major, DEVICE_NAME);
		return -1;
	}
	optical_dev = device_create(optical_class,\
									  NULL, \
									  MKDEV(optical_major, 0), \
									  NULL,\
									  DEVICE_NAME);
	if(IS_ERR(optical_dev))
	{
		printk("optical: failed in creating device!\n");
		unregister_chrdev(optical_major, DEVICE_NAME);
		class_unregister(optical_class);
		class_destroy(optical_class);
		return -1;
	}
	base_addr = (unsigned long) ioremap(OPTICAL_BASEADDR, sizeof(u32));
	printk("Optical initial successfully\n");
	return 0;
}
static void __exit optical_exit(void)
{
  device_destroy(optical_class, MKDEV(optical_major, 0));
  class_unregister(optical_class);
  class_destroy(optical_class);
  unregister_chrdev(optical_major, DEVICE_NAME);
  printk("optical module exit.\n");
}
module_init(optical_init);
module_exit(optical_exit);
