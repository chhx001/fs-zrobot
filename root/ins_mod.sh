#! /bin/sh -
export PATH=$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
if [ "`lsmod | grep mt7601`x" = "x" ]
then
	insmod /etc/Wireless/RT2870STA/mt7601Usta.ko
	if [ "`lsmod | grep mt7601`x" = "x" ]
	then
		echo "360 Wifi init failed!"
	else
		echo "360 Wifi init Successful!"
	fi
else
	echo "You 360wifi already inserted!"
	exit 1
fi

./root/zrobot_drivers/car_drv.sh

#insmod /root/zrobot_drivers/optical_driver.ko
#insmod /root/zrobot_drivers/wheel_driver.ko
#insmod /root/zrobot_drivers/ultrasonic_driver.ko
#insmod /root/zrobot_drivers/uarm_driver.ko

./root/wlan_ap.sh
./root/mjpg_start.sh &
./root/APPForCar/zrtcp_app/tcp_server &

