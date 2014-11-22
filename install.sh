#!/bin/sh

## Dependencies
sudo apt-get update

## GUI
sudo apt-get install python-tk -y
sudo apt-get install unclutter -y
sudo apt-get install python-imaging-tk -y

## Splash Screen
sudo apt-get install fbi -y
sudo cp images/splash.png /etc/
sudo cp config/asplashscreen /etc/init.d/
sudo chmod a+x /etc/init.d/asplashscreen
sudo insserv asplashscreen

## Boot
sudo cp config/cmdline.txt /boot/
sudo chmod a+x /boot/cmdline.txt

## Networking (Ad-Hoc and cross-over Ethernet)
sudo apt-get install isc-dhcp-server -y
sudo cp config/interfaces /etc/network/

# Fullscreen
sudo cp config/autostart /etc/xdg/lxsession/LXDE/autostart 
sudo cp config/lightdm.conf /etc/lightdm/lightdm.conf

# Arduino
sudo apt-get install python-serial arduino arduino-mk -y
sudo cp config/avrdude /usr/bin/avrdude
sudo cp config/avrdude /usr/share/arduino/hardware/tools
sudo cp config/avrdude.conf  /usr/share/arduino/hardware/tools
sudo cp config/boards.txt  /usr/share/arduino/hardware/arduino
sudo cp config/inittab /etc
sudo cp config/80-alamode.rules /etc/udev/rules.d
sudo chown root /usr/bin/avrdude /usr/share/arduino/hardware/tools/avrdude
sudo chgrp root /usr/bin/avrdude /usr/share/arduino/hardware/tools/avrdude
sudo chmod a+s /usr/bin/avrdude /usr/share/arduino/hardware/tools/avrdude
sudo cp -r libs/* /usr/share/arduino/libraries
sudo cp  config/init-functions /lib/lsb/init-functions
sudo cp config/dphys-swapfile(1) /etc/init.d/dphys-swapfile
sudo cp config/dphys-swapfile /sbin/dphys-swapfile

