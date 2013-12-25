#!/usr/bin

## Dependencies
sudo apt-get update
sudo apt-get upgrade -y
sudo apt-get install python-serial -y
sudo apt-get install python-tkinter -y

## Splash Screen
sudo apt-get install fbi
sudo cp config/asplashscreen /etc/init.d/
sudo chmod a+x /etc/init.d/asplashscreen
insserv asplashscreen

## Boot to Fullscreen
sudo echo "@xset s off" > /etc/xdg/lxsession/LXDE/autostart 
sudo echo "@xset -dpms" >> /etc/xdg/lxsession/LXDE/autostart 
sudo echo "@xset s noblank" >> /etc/xdg/lxsession/LXDE/autostart 
sudo echo "@python ~/MR15/MR15.py" >> /etc/xdg/lxsession/LXDE/autostart 

## Kernel Tweaks
sudo echo "dwc_otg.lpm_enable=0 root=/dev/mmcblk0p2 rootfstype=ext4 rootflags=commit=120,data=writeback elevator=deadline noatime nodiratime data=writeback rootwait quiet" > /boot/cmdline.txt

## Disable Getty
sed -i '/[2-6]:23:respawn:\/sbin\/getty 38400 tty[2-6]/s%^%#%g' /etc/inittab