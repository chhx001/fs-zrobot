#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "gpio_cmd.h"

#define DEVICE_NAME "zrcar_gpio_dev"
#define GPIO_BASEADDR 0x41220000

#define GPIO_CN1_OFFSET 0x0
#define GPIO_CN2_OFFSET 0x8
#define GPIO_CN1_DIR_OFFSET 0x4
#define GPIO_CN2_DIR_OFFSET 0xC
#define GPIO_DATA_OFFSET(x) ((x) / 8 * 8)
#define GPIO_DIR_OFFSET(x) (GPIO_DATA_OFFSET(x) + 4)

#define STATUS_CN1 (gpio_status & 0xff)
#define STATUS_CN2 ((gpio_status & 0x3f00) >> 8)

#define MAXNUM 13
#define MINNUM 0
#define CONSTRAIN(x,min,max) (x) = (((x) > (max))?(max):((x) < (min))?(min):(x))

MODULE_AUTHOR("ZRobot");
MODULE_DESCRIPTION("driver for gpio-controling of the car");
MODULE_VERSION("v1.0");
MODULE_LICENSE("GPL");

static int gpio_major;
static struct class* gpio_class = NULL;
static struct device* gpio_device = NULL;

unsigned long gpio_baseaddr = 0;
unsigned int gpio_status = 0;

void gpio_set(int status)
{
	gpio_status = status;
	iowrite32(0x00,gpio_baseaddr + GPIO_CN1_DIR_OFFSET);
	iowrite32(0x00,gpio_baseaddr + GPIO_CN2_DIR_OFFSET);
	iowrite32(STATUS_CN1,gpio_baseaddr + GPIO_CN1_OFFSET);
	iowrite32(STATUS_CN2,gpio_baseaddr + GPIO_CN2_OFFSET);
}


void gpio_set_up(int num)
{
	CONSTRAIN(num,MINNUM,MAXNUM);
	gpio_status |= 1 << num;
	iowrite32(0x00,gpio_baseaddr + GPIO_DIR_OFFSET(num));
	iowrite32((num / 8)?STATUS_CN2:STATUS_CN1,gpio_baseaddr + GPIO_DATA_OFFSET(num));
}

void gpio_set_down(int num)
{
	CONSTRAIN(num,MINNUM,MAXNUM);
	gpio_status &= ~(1 << num);
	iowrite32(0x00,gpio_baseaddr + GPIO_DIR_OFFSET(num));
	iowrite32((num / 8)?STATUS_CN2:STATUS_CN1,gpio_baseaddr + GPIO_DATA_OFFSET(num));
}

u32 gpio_get()
{
	iowrite32(0xff,gpio_baseaddr + GPIO_CN1_OFFSET);
	iowrite32(0xff,gpio_baseaddr + GPIO_CN2_OFFSET);

	gpio_status = (ioread32(gpio_baseaddr + GPIO_CN1_OFFSET) & 0xff) + ((ioread32(gpio_baseaddr + GPIO_CN2_OFFSET) & 0x3f) << 8);
	return gpio_status;
}

static ssize_t gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    u32 status = 0xff;
    int ret;
    switch(cmd){
        case GPIO_UP:
			gpio_set_up((int)arg);
			break;
		case GPIO_DOWN:
			gpio_set_down((int)arg);
			break;
		case GPIO_GET:
			status = gpio_get();
			__put_user(status,(u32 *)arg);
			break;
		case GPIO_SET:
			gpio_set((int)arg);
			break;
		default:
			printk("Invalid command,cmd=%d\n",cmd);
			return -EINVAL;
	}

	return 0;

}


static int gpio_open(struct inode *inode, struct file *filp)
{
		
    return 0;
}

static struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
	.unlocked_ioctl = gpio_ioctl,
	.open = gpio_open,
};

static int __init zrcar_gpio_module_init(void)
{
    gpio_major=register_chrdev(0, DEVICE_NAME, &gpio_fops);
    if (gpio_major < 0){
        printk("failed to register device.\n");
        return -1;
    }

    gpio_class = class_create(THIS_MODULE, "gpio_class");
    if (IS_ERR(gpio_class)){
        printk("failed to create device class.\n");
        unregister_chrdev(gpio_major, DEVICE_NAME);
        return -1;
    }


    gpio_device = device_create(gpio_class, NULL, MKDEV(gpio_major, 0), NULL, "zrcar_gpio_dev");
    if (IS_ERR(gpio_device)){
        printk("failed to create device .\n");
        unregister_chrdev(gpio_major, DEVICE_NAME);
        return -1;
    }

    gpio_baseaddr = (unsigned long)ioremap(GPIO_BASEADDR, 16);

    printk("gpio driver initial successfully!\n");
    return 0;
}

static void __exit zrcar_gpio_module_exit(void)
{
    device_destroy(gpio_class, MKDEV(gpio_major, 0));
    class_unregister(gpio_class);
    class_destroy(gpio_class);
    unregister_chrdev(gpio_major, DEVICE_NAME);
    printk("gpio module exit.\n");
}

module_init(zrcar_gpio_module_init);
module_exit(zrcar_gpio_module_exit);
