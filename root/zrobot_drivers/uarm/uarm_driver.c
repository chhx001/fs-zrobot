#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>

#include "uarm_cmd.h"

#define DEVICE_NAME "zrarm_uarm_dev"
//#define IIC_BASEADDR 0x41220000
#define UARM_BASEADDR 0x43c20000
#define DETACH_BASEADDR 0x41240000
#define BTN_BASEADDR 0x41230000
#define AD_BASEADDR 0x43c30000

#define UARM_SERVO_L 0
#define UARM_SERVO_R 1
#define UARM_SERVO_ROT 2
#define UARM_SERVO_HANDROT 3
#define UARM_SERVO_HAND 4
#define UARM_PUMP_EN 5
#define UARM_VALVE_EN 6
#define UARM_LIMIT_SW 7


#define UARM_CHANNEL_BASE(baseaddr,chan) ((baseaddr) + (chan) * 4)

#define GPIO_CHANNEL_BASE(baseaddr,chan) ((baseaddr) + (chan - 1) * 8)
#define GPIO_DIR_BASE(baseaddr) ((baseaddr) + 4)


#define DIR_OUT 0
#define DIR_IN 1

#define SDA1 iic_sda_set(1)
#define SDA0 iic_sda_set(0)
#define SDAOUT iic_sda_set_dir(DIR_OUT)
#define SDAIN iic_sda_set_dir(DIR_IN)
#define SDADATA iic_sda_data()

#define SCL1 iic_scl_set(1)
#define SCL0 iic_scl_set(0)
#define SCLOUT iic_scl_set_dir(DIR_OUT)

#define IIC_DELAY udelay(25);
#define SLAVE_ADDR 0xa0

#define SCL_CHANNEL 1
#define SDA_CHANNEL 2


MODULE_DESCRIPTION("driver for uarm-controling of the car");
MODULE_VERSION("v1.0");
MODULE_LICENSE("GPL");

static int uarm_major;
static struct class* uarm_class = NULL;
static struct device* uarm_device = NULL;

//unsigned long iic_baseaddr = 0;
unsigned long uarm_baseaddr = 0;
unsigned long detach_baseaddr = 0;
unsigned long btn_baseaddr = 0;
unsigned long ad_baseaddr = 0;

/*void iic_scl_set(int tag) 
{
	if(tag)
		iowrite32(1,GPIO_CHANNEL_BASE(iic_baseaddr,SCL_CHANNEL));
	else
		iowrite32(0,GPIO_CHANNEL_BASE(iic_baseaddr,SCL_CHANNEL));
}

void iic_sda_set(int tag)
{
	if(tag)
		iowrite32(1,GPIO_CHANNEL_BASE(iic_baseaddr,SDA_CHANNEL));
	else
		iowrite32(0,GPIO_CHANNEL_BASE(iic_baseaddr,SDA_CHANNEL));
}

void iic_scl_set_dir(int tag)
{
	if(tag)
		iowrite32(DIR_IN,GPIO_DIR_BASE(GPIO_CHANNEL_BASE(iic_baseaddr,SCL_CHANNEL)));
	else
		iowrite32(DIR_OUT,GPIO_DIR_BASE(GPIO_CHANNEL_BASE(iic_baseaddr,SCL_CHANNEL)));
}

void iic_sda_set_dir(int tag)
{
	if(tag)
		iowrite32(DIR_IN,GPIO_DIR_BASE(GPIO_CHANNEL_BASE(iic_baseaddr,SDA_CHANNEL)));
	else
		iowrite32(DIR_OUT,GPIO_DIR_BASE(GPIO_CHANNEL_BASE(iic_baseaddr,SDA_CHANNEL)));
}

int iic_sda_data()
{
	return ioread32(GPIO_CHANNEL_BASE(iic_baseaddr,SDA_CHANNEL));
}

void iic_start()
{
	SDA1;
	SCL1;
	IIC_DELAY;
	SDA0;
	IIC_DELAY;
	SCL0;
	IIC_DELAY;
}

void iic_stop()
{
	SDA0;
	SCL1;
	IIC_DELAY;
	SDA1;
	IIC_DELAY;
}

void iic_send_ack(unsigned char ack)
{
	SDAOUT;
	if(ack)	SDA1;
	else	SDA0;
	SCL1;
	IIC_DELAY;
	SCL0;
	IIC_DELAY;
}

unsigned char iic_recv_ack()
{
	unsigned char cy;
	SDAIN;
	SCL1;
	IIC_DELAY;
	if(SDADATA)	cy=1;
	else		cy=0;
	SCL0;
	IIC_DELAY;
	SDAOUT;
	return cy;
}

void iic_send_byte(unsigned char dat)
{
	unsigned char i;
	SDAOUT;
	for (i = 0;i < 8; ++ i){
		if((dat<<i)&0x80){
			SDA1;
		}
		else{
			SDA0;
		}
		SCL1;
		IIC_DELAY;
		SCL0;
		IIC_DELAY;
	}
	iic_recv_ack();
}

unsigned char iic_recv_byte()
{
	unsigned char i;
	unsigned char dat = 0,cy;
	SDA1;
	SDAIN;
	for(i = 0;i < 8;++ i){
		dat <<= 1;
		SCL1;
		IIC_DELAY;
		if(SDADATA){
			cy=1;
		}
		else{
			cy=0;
		}
		dat |= cy;
		SCL0;
		IIC_DELAY;
	}
	SDAOUT;
	return dat;
}

void single_iic_write(unsigned char addr,unsigned char data)
{
	unsigned long flag;
	//irq save
	local_irq_save(flag);
	iic_start();
	iic_send_byte(SLAVE_ADDR);
	iic_send_byte(addr);
	iic_send_byte(data);
	iic_stop();
	//irq restore
	local_irq_restore(flag);
}

unsigned char single_iic_read(unsigned char addr)
{
	unsigned long flag;
	unsigned char data;
	local_irq_save(flag);
	iic_start();
	iic_send_byte(SLAVE_ADDR);
	iic_send_byte(addr);
	iic_start();
	iic_send_byte(SLAVE_ADDR + 1);
	data = iic_recv_byte();
	iic_send_ack(1);
	iic_stop();
	local_irq_restore(flag);
	return data;
}*/

void uarm_reg_write(struct ZRUarmReg *reg)
{
	iowrite32(reg->value,UARM_CHANNEL_BASE(uarm_baseaddr,reg->channel));
}

void uarm_reg_read(struct ZRUarmReg *reg)
{
	reg->value = ioread32(UARM_CHANNEL_BASE(uarm_baseaddr,reg->channel));
}

void ad_read(struct ZRADAccess *ad_access)
{
	ad_access->value = ioread32(UARM_CHANNEL_BASE(ad_baseaddr,ad_access->channel));
}

void btn_read(struct ZRUarmReg *reg)
{
	reg->value = ioread32(GPIO_CHANNEL_BASE(btn_baseaddr,reg->channel));
}


static ssize_t uarm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("uarm ioctl\ncmd=%d\n",cmd);
    u32 status = 0xff;
    char buf[16];
	struct ZRUarmReg *servo_value;
	struct ZRADAccess *ad_access;
	struct ZRIICAccess *iic_access;
    switch(cmd){
        case UARM_INIT:
			
			break;
		case UARM_REG_WRITE:
			copy_from_user(buf,(void __user*)arg,sizeof(struct ZRUarmReg));
			servo_value = (struct ZRUarmReg*)buf;
			uarm_reg_write(servo_value);
			break;
		case UARM_AD_READ:
			copy_from_user(buf,(void __user*)arg,sizeof(struct ZRADAccess));
			ad_access = (struct ZRADAccess*)buf;
			ad_read(ad_access);
			copy_to_user(arg,buf,sizeof(struct ZRADAccess));
			break;
		case UARM_REG_READ:
			copy_from_user(buf,(void __user*)arg,sizeof(struct ZRUarmReg));
			servo_value = (struct ZRUarmReg*)buf;
			uarm_reg_read(servo_value);
			copy_to_user(arg,buf,sizeof(struct ZRUarmReg));
			break;
		case UARM_BTN_READ:
			copy_from_user(buf,(void __user*)arg,sizeof(struct ZRUarmReg));
			servo_value = (struct ZRUarmReg*)buf;
			btn_read(servo_value);
			copy_to_user(arg,buf,sizeof(struct ZRUarmReg));
			break;
		case UARM_ATTACH:
			iowrite32(1,detach_baseaddr);
			break;
		case UARM_DETACH:
			iowrite32(0,detach_baseaddr);
			break;
		default:
			printk("Invalid command:cmd=%d\n",cmd);
			return -EINVAL;
	}

	return 0;

}

void uarm_init(void)
{
	iowrite32(DIR_IN,GPIO_DIR_BASE(GPIO_CHANNEL_BASE(btn_baseaddr,1)));
	iowrite32(DIR_IN,GPIO_DIR_BASE(GPIO_CHANNEL_BASE(btn_baseaddr,2)));
	iowrite32(0,GPIO_DIR_BASE(detach_baseaddr));
	iowrite32(1,detach_baseaddr);
}

static int uarm_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static struct file_operations uarm_fops = {
    .owner = THIS_MODULE,
	.unlocked_ioctl = uarm_ioctl,
	.open = uarm_open,
};

static int __init zrarm_uarm_module_init(void)
{
    uarm_major=register_chrdev(0, DEVICE_NAME, &uarm_fops);
    if (uarm_major < 0){
        printk("failed to register device.\n");
        return -1;
    }

    uarm_class = class_create(THIS_MODULE, "uarm_class");
    if (IS_ERR(uarm_class)){
        printk("failed to create device class.\n");
        unregister_chrdev(uarm_major, DEVICE_NAME);
        return -1;
    }


    uarm_device = device_create(uarm_class, NULL, MKDEV(uarm_major, 0), NULL, "zrarm_uarm_dev");
    if (IS_ERR(uarm_device)){
        printk("failed to create device .\n");
        unregister_chrdev(uarm_major, DEVICE_NAME);
        return -1;
    }

    uarm_baseaddr = (unsigned long)ioremap(UARM_BASEADDR, 16);
   // iic_baseaddr = (unsigned long)ioremap(IIC_BASEADDR, 16);
	detach_baseaddr = (unsigned long)ioremap(DETACH_BASEADDR,16);
	btn_baseaddr = (unsigned long)ioremap(BTN_BASEADDR,16);
	ad_baseaddr = (unsigned long)ioremap(AD_BASEADDR,16);
	
	uarm_init();
    printk("my uarm driver initial successfully!\n");
    return 0;
}

static void __exit zrarm_uarm_module_exit(void)
{
    device_destroy(uarm_class, MKDEV(uarm_major, 0));
    class_unregister(uarm_class);
    class_destroy(uarm_class);
    unregister_chrdev(uarm_major, DEVICE_NAME);
    printk("uarm module exit.\n");
}

module_init(zrarm_uarm_module_init);
module_exit(zrarm_uarm_module_exit);
