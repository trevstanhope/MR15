#!/bin/sh

## Dependencies
sudo apt-get update
sudo apt-get install python-serial -y
sudo apt-get install python-tk -y
sudo apt-get install unclutter -y

## Splash Screen
sudo apt-get install fbi
sudo cp images/splash.png /etc/
sudo cp config/asplashscreen /etc/init.d/
sudo chmod a+x /etc/init.d/asplashscreen
sudo insserv asplashscreen