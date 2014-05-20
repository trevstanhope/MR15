#!/bin/sh

## Dependencies
sudo apt-get update
sudo apt-get install python-serial -y
sudo apt-get install python-tk -y
sudo apt-get install unclutter -y
sudo apt-get install python-imaging-tk

## Splash Screen
sudo apt-get install fbi
sudo cp images/splash.png /etc/
sudo cp config/asplashscreen /etc/init.d/
sudo chmod a+x /etc/init.d/asplashscreen
sudo insserv asplashscreen

## Boot
sudo cp config/cmdline.txt /boot/
sudo chmod a+x /boot/cmdline.txt

## Ad-Hoc
sudo apt-get install isc-dhcp-server -y
#sudo cp config/interfaces /etc/network/

# Fullscreen
sudo cp config/autostart /etc/xdg/lxsession/LXDE/autostart 
sudo cp config/lightdm.conf /etc/lightdm/lightdm.conf

# Alamode
sudo apt-get install arduino arduino-mk
sudo cp configs/avrdude /usr/bin/avrdude
sudo cp configs/avrdude /usr/share/arduino/hardware/tools
sudo cp configs/avrdude.conf  /usr/share/arduino/hardware/tools
sudo cp configs/boards.txt  /usr/share/arduino/hardware/arduino
sudo cp configs/cmdline.txt /boot
sudo cp configs/inittab /etc
sudo cp configs/80-alamode.rules /etc/udev/rules.d
sudo chown root /usr/bin/avrdude /usr/share/arduino/hardware/tools/avrdude
sudo chgrp root /usr/bin/avrdude /usr/share/arduino/hardware/tools/avrdude
sudo chmod a+s /usr/bin/avrdude /usr/share/arduino/hardware/tools/avrdude
sudo cp -r libs/* /usr/share/arduino/libraries
