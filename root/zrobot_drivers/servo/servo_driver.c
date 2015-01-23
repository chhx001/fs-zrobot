#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "servo_cmd.h"

#define DEVICE_NAME "zrcar_servo_dev"
#define SERVO_BASEADDR 0x43C20000

#define SERVO0_OFFSET 0x0
#define SERVO1_OFFSET 0x4

MODULE_AUTHOR("ZRobot");
MODULE_DESCRIPTION("driver for servo-controling of the car");
MODULE_VERSION("v1.0");
MODULE_LICENSE("GPL");

static int servo_major;
static struct class* servo_class = NULL;
static struct device* servo_device = NULL;

unsigned long servo_baseaddr = 0;

void servo1_set(int angle);
void servo0_set(int angle);

void servo1_set(int angle)
{
	iowrite32(angle,servo_baseaddr + SERVO1_OFFSET);
}

void servo0_set(int angle)
{
	iowrite32(angle,servo_baseaddr + SERVO0_OFFSET);
}

static ssize_t servo_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    u32 status = 0xff;
    int ret;
    switch(cmd){
        case SERVO0_SET:
			servo0_set((int)arg);
			break;
		case SERVO1_SET:
			servo1_set((int)arg);
			break;
		default:
			printk("Invalid command,cmd=%d\n",cmd);
			return -EINVAL;
	}

	return 0;

}


static int servo_open(struct inode *inode, struct file *filp)
{
		
    return 0;
}

static struct file_operations servo_fops = {
    .owner = THIS_MODULE,
	.unlocked_ioctl = servo_ioctl,
	.open = servo_open,
};

static int __init zrcar_servo_module_init(void)
{
    servo_major=register_chrdev(0, DEVICE_NAME, &servo_fops);
    if (servo_major < 0){
        printk("failed to register device.\n");
        return -1;
    }

    servo_class = class_create(THIS_MODULE, "servo_class");
    if (IS_ERR(servo_class)){
        printk("failed to create device class.\n");
        unregister_chrdev(servo_major, DEVICE_NAME);
        return -1;
    }


    servo_device = device_create(servo_class, NULL, MKDEV(servo_major, 0), NULL, "zrcar_servo_dev");
    if (IS_ERR(servo_device)){
        printk("failed to create device .\n");
        unregister_chrdev(servo_major, DEVICE_NAME);
        return -1;
    }

    servo_baseaddr = (unsigned long)ioremap(SERVO_BASEADDR, 16);

    printk("servo driver initial successfully!\n");
    return 0;
}

static void __exit zrcar_servo_module_exit(void)
{
    device_destroy(servo_class, MKDEV(servo_major, 0));
    class_unregister(servo_class);
    class_destroy(servo_class);
    unregister_chrdev(servo_major, DEVICE_NAME);
    printk("servo module exit.\n");
}

module_init(zrcar_servo_module_init);
module_exit(zrcar_servo_module_exit);
