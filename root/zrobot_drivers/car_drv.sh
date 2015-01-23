#!/bin/bash
echo "Inserting drivers..."
err=`insmod /root/zrobot_drivers/wheel/wheel_driver.ko 2>&1`
if [ -n "$err" ];then
	echo "$err"
	echo "Insert wheel driver failed!"
	iferr="true"
fi
err=`insmod /root/zrobot_drivers/ultrasonic/ultrasonic_driver.ko 2>&1`
if [ -n "$err" ];then
	echo "$err"
	echo "Insert ultrasonic driver failed!"
	iferr="true"
fi
err=`insmod /root/zrobot_drivers/optical/optical_driver.ko 2>&1`
if [ -n "$err" ];then
	echo "$err"
	echo "Insert optical driver failed!"
	iferr="true"
fi

err=`insmod /root/zrobot_drivers/gpio/gpio_driver.ko 2>&1`
if [ -n "$err" ];then
	echo "$err"
	echo "Insert gpio driver failed!"
	iferr="true"
fi
#err=`insmod drivers/servo/servo_driver.ko 2>&1`
#if [ -n "$err" ];then
#	echo "$err"
#	echo "Insert servo driver failed!"
#	iferr="true"
#fi

if [ -n "$iferr" ];then
	echo "There are some errors while inserting drivers..."
else
	echo "Insert drivers succeeded!"
fi

