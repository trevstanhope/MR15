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
sudo cp config/interfaces /etc/network/
