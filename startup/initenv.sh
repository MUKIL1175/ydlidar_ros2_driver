#!/bin/bash
# YDLIDAR udev rules setup script
# Compatible with Ubuntu 20.04 / 22.04 / 24.04

echo 'KERNEL=="ttyUSB*", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE:="0666", GROUP:="dialout", SYMLINK+="ydlidar"' >/etc/udev/rules.d/ydlidar.rules
echo 'KERNEL=="ttyACM*", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="5740", MODE:="0666", GROUP:="dialout", SYMLINK+="ydlidar"' >/etc/udev/rules.d/ydlidar-V2.rules
echo 'KERNEL=="ttyUSB*", ATTRS{idVendor}=="067b", ATTRS{idProduct}=="2303", MODE:="0666", GROUP:="dialout", SYMLINK+="ydlidar"' >/etc/udev/rules.d/ydlidar-2303.rules

if command -v udevadm >/dev/null 2>&1; then
    udevadm control --reload-rules
    udevadm trigger
else
    service udev reload
    sleep 1
    service udev restart
fi

echo "YDLIDAR udev rules installed successfully."
echo "Please unplug and re-plug your LiDAR USB cable to apply."
