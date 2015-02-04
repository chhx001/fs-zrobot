#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "wheel_cmd.h"

#define DEVICE_NAME "zrcar_wheel_dev"
#define WHEEL_L_BASEADDR 0x43C01000
#define WHEEL_R_BASEADDR 0x43C02000

#define CTRL_OFFSET 0 
#define SPEED_SET_OFFSET 1 * 4
#define KP_PARAM_OFFSET 2 * 4
#define KI_PARAM_OFFSET 3 * 4
#define PID_OUT_MAX_OFFSET 4 * 4
#define PID_OUT_MIN_OFFSET 5 * 4
#define PID_UI_LIMIT_OFFSET 6 * 4

#define RESET_OFFSET 14 * 4
#define SPEED_GET_OFFSET 15 * 4

/*#define MAGIC 'w'
#define WHEEL_INIT	_IOW(MAGIC,0,int)
#define WHEEL_L_SET	_IOW(MAGIC,1,int)
#define WHEEL_R_SET	_IOW(MAGIC,2,int)
#define WHEEL_L_GET	_IOR(MAGIC,3,int)
#define WHEEL_R_GET	_IOR(MAGIC,4,int)
*/


MODULE_AUTHOR("ZRobot");
MODULE_DESCRIPTION("driver for wheel-controling of the car");
MODULE_VERSION("v1.0");
MODULE_LICENSE("GPL");

static int wheel_major;
static struct class* wheel_class = NULL;
static struct device* wheel_device = NULL;

unsigned long wheel_l_baseaddr = 0;
unsigned long wheel_r_baseaddr = 0;

void wheel_init(void);
void wheel_l_speed_set(int speed);
void wheel_r_speed_set(int speed);
int wheel_l_speed_get(void);
int wheel_r_speed_get(void);

void wheel_l_speed_set(int speed)
{
	printk("set l speed:%d\n",-speed);
	iowrite32(-speed,wheel_l_baseaddr + SPEED_SET_OFFSET);
}

void wheel_r_speed_set(int speed)
{
	iowrite32(-speed,wheel_r_baseaddr + SPEED_SET_OFFSET);
}

int wheel_l_speed_get(void)
{
	return -(int)(ioread32(wheel_l_baseaddr + SPEED_GET_OFFSET));
}

int wheel_r_speed_get(void)
{
	return -(int)(ioread32(wheel_r_baseaddr + SPEED_GET_OFFSET));
}

static ssize_t wheel_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("wheel ioctl\ncmd=%d\n",cmd);
    u32 status = 0xff;
    int ret;
    switch(cmd){
        case WHEEL_INIT:
			break;
		case WHEEL_L_SET:
			wheel_l_speed_set((int)arg);
			break;
		case WHEEL_L_GET:
			status = wheel_l_speed_get();
			ret = __put_user(status, (long *)arg); 
			break;
		case WHEEL_R_SET:
			wheel_r_speed_set((int)arg);
			break;
		case WHEEL_R_GET:
			status = wheel_r_speed_get();
			ret = __put_user(status, (long *)arg); 
			break;
		default:
			printk("Invalid command:cmd=%d\n",cmd);
			return -EINVAL;
	}

	return 0;

}

void wheel_init(void)
{
	iowrite32(0 ,wheel_l_baseaddr + CTRL_OFFSET);
	iowrite32(-0 ,wheel_l_baseaddr + SPEED_SET_OFFSET);
	iowrite32(1<<8 ,wheel_l_baseaddr + KP_PARAM_OFFSET);
	iowrite32(1<<8 ,wheel_l_baseaddr + KI_PARAM_OFFSET);
	iowrite32(63 ,wheel_l_baseaddr + PID_OUT_MAX_OFFSET );
	iowrite32(-63 ,wheel_l_baseaddr + PID_OUT_MIN_OFFSET );
	iowrite32(70<<8 ,wheel_l_baseaddr + PID_UI_LIMIT_OFFSET );

	iowrite32(0 ,wheel_l_baseaddr + RESET_OFFSET );
	iowrite32(0x0ff ,wheel_l_baseaddr + RESET_OFFSET );

	iowrite32(1 ,wheel_l_baseaddr + CTRL_OFFSET);

	/***************right********************/

	iowrite32(0 ,wheel_r_baseaddr + CTRL_OFFSET);
	iowrite32(-0 ,wheel_r_baseaddr + SPEED_SET_OFFSET);
	iowrite32(1<<8 ,wheel_r_baseaddr + KP_PARAM_OFFSET);
	iowrite32(1<<8 ,wheel_r_baseaddr + KI_PARAM_OFFSET);
	iowrite32(63 ,wheel_r_baseaddr + PID_OUT_MAX_OFFSET );
	iowrite32(-63 ,wheel_r_baseaddr + PID_OUT_MIN_OFFSET );
	iowrite32(70<<8 ,wheel_r_baseaddr + PID_UI_LIMIT_OFFSET );

	iowrite32(0 ,wheel_r_baseaddr + RESET_OFFSET );
	iowrite32(0x0ff ,wheel_r_baseaddr + RESET_OFFSET );

	iowrite32(1 ,wheel_r_baseaddr + CTRL_OFFSET);


}

static int wheel_open(struct inode *inode, struct file *filp)
{
    printk("wheel open and init\n");
		
    return 0;
}

static struct file_operations wheel_fops = {
    .owner = THIS_MODULE,
	.unlocked_ioctl = wheel_ioctl,
	.open = wheel_open,
};

static int __init zrcar_wheel_module_init(void)
{
    wheel_major=register_chrdev(0, DEVICE_NAME, &wheel_fops);
    if (wheel_major < 0){
        printk("failed to register device.\n");
        return -1;
    }

    wheel_class = class_create(THIS_MODULE, "wheel_class");
    if (IS_ERR(wheel_class)){
        printk("failed to create device class.\n");
        unregister_chrdev(wheel_major, DEVICE_NAME);
        return -1;
    }


    wheel_device = device_create(wheel_class, NULL, MKDEV(wheel_major, 0), NULL, "zrcar_wheel_dev");
    if (IS_ERR(wheel_device)){
        printk("failed to create device .\n");
        unregister_chrdev(wheel_major, DEVICE_NAME);
        return -1;
    }

    wheel_l_baseaddr = (unsigned long)ioremap(WHEEL_L_BASEADDR, 16);
    wheel_r_baseaddr = (unsigned long)ioremap(WHEEL_R_BASEADDR, 16);

	wheel_init();
    printk("my wheel driver initial successfully!\n");
    return 0;
}

static void __exit zrcar_wheel_module_exit(void)
{
    device_destroy(wheel_class, MKDEV(wheel_major, 0));
    class_unregister(wheel_class);
    class_destroy(wheel_class);
    unregister_chrdev(wheel_major, DEVICE_NAME);
    printk("wheel module exit.\n");
}

module_init(zrcar_wheel_module_init);
module_exit(zrcar_wheel_module_exit);
