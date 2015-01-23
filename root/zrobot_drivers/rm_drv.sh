#!/bin/bash
echo "Removing existed drivers..."
(rmmod wheel_driver) 2>/dev/null
(rmmod optical_driver) 2>/dev/null
(rmmod ultrasonic_driver) 2>/dev/null
(rmmod gpio_driver) 2>/dev/null
(rmmod uarm_driver) 2>/dev/null
echo "Existed driver removed!"
