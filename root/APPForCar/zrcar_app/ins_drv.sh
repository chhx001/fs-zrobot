#!/bin/bash
echo "Inserting drivers..."
err=`insmod drivers/wheel/wheel_driver.ko 2>&1`
if [ -n "$err" ];then
	echo "$err"
	echo "Insert wheel driver failed!"
	iferr="true"
fi
err=`insmod drivers/ultrasonic/ultrasonic_driver.ko 2>&1`
if [ -n "$err" ];then
	echo "$err"
	echo "Insert ultrasonic driver failed!"
	iferr="true"
fi
err=`insmod drivers/optical/optical_driver.ko 2>&1`
if [ -n "$err" ];then
	echo "$err"
	echo "Insert optical driver failed!"
	iferr="true"
fi
if [ -n "$iferr" ];then
	echo "There are some errors while inserting drivers..."
else
	echo "Insert drivers succeeded!"
fi

