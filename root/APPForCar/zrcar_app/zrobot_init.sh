#!/bin/bash
if [ $# -eq 0 ];then
	cmd="-h"
else
	cmd=$1
fi

if [ $cmd = "-h" ] || [ $cmd = "--help" ];then
	echo "ZRobot API v1.0"
	#echo "ZRobot API Initialize Script"
	echo "Usage:"
	echo "     ./zrobot_init.sh [option] "
	echo "options:"
	echo "-v            Print details of this API"
	echo "-a            Initialize the environment for this API"
	echo "-i            Insert the drivers of this API"
	echo "-r            Remove the drivers of this API"
	echo "-h or --help  Print the help of this script"

elif [ $cmd = "-a" ];then
	echo "Initializing environment..."
	./rm_drv.sh
	./ins_drv.sh
	echo "Environment Initialized."
elif [ $cmd = "-i" ];then
	./ins_drv.sh
elif [ $cmd = "-r" ];then
	./rm_drv.sh
elif [ $cmd = "-v" ];then
	echo "ZRobot API version 1.0 released in 2015-1-5"
else
	echo "Invalid Command,please use -h or --help for help."
fi

