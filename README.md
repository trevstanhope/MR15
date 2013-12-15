# Mutrac MR15

## Sub-Systems

### Steering
Steering must be available whenever the vehicle is ON.
This requires an Arduino which should run independently of all other processes.
The steering sub-system would consist of three components
1. Actuator (steering)
2. Steering Input (steering wheel)
3. Steering Position (steering potentiometer)

### CVT
The CVT requires input from multiple sensors for optimization.
Therefore, this subsystem cannot be independent from the sensors.
1. Ballast Motor
2. CVT Actuator

### Engine and Safety
1. Killswitch seat
2. Killswitch button
3. Killswitch hitch
4. Lockswitch brakes
5. Lockswitch guard
6. Ignition

### Sensors
1. Front Wheel RPM
2. Rear Wheel RPM
3. Fuel rate1
4. Belt 1 RPM
5. Belt 2 RPM

## Installing the Splash Screen
Install FBI

    apt-get install fbi
    
Copy your custom splash image to /etc/ and name it "splash.png".
Next, copy the init.d script called "asplashscreen" from "config/" into "/etc/init.d/".
Make it executable

    chmod a+x /etc/init.d/asplashscreen
    
Enable the script as at runtime

    insserv asplashscreen
    
Then reboot the system

    reboot
    
## Configuring Boot to Fullscreen
Edit the LDM config:

    sudo nano /etc/lightdm/lightdm.conf
    
Add the following lines to the [SeatDefaults] section:

    xserver-command=X -s 0 dpms
    
Hide cursor:

    sudo apt-get install unclutter
    
Open LXDE configuration file:

    sudo nano /etc/xdg/lxsession/LXDE/autostart 
    
Comment everything and add the following lines:

    @xset s off
    @xset -dpms
    @xset s noblank
    @python ~/MR15/examples/fullscreen_tkinter.py # will change later to MR15.py
    
## Kernel
Open /boot/cmdline.txt:
  
    sudo nano /boot/cmdline.txt
    
/boot/cmdline.txt

    dwc_otg.lpm_enable=0 root=/dev/mmcblk0p2 rootfstype=ext4 rootflags=commit=120,data=writeback elevator=deadline noatime  nodiratime  data=writeback rootwait quiet
    
## System
sudo nano /etc/sysctl.conf
    
    vm.dirty_background_ratio = 20
    vm.dirty_expire_centisecs = 0
    vm.dirty_ratio = 80
    vm.dirty_writeback_centisecs = 1200
    vm.overcommit_ratio = 2
    vm.laptop_mode = 5
    vm.swappiness = 10

## Disable Getty
sed -i '/[2-6]:23:respawn:\/sbin\/getty 38400 tty[2-6]/s%^%#%g' /etc/inittab

## Broadcom
sudo modprobe -r snd-bcm2835

## Swap
echo "CONF_SWAPSIZE=512" > /etc/dphys-swapfile
dphys-swapfile setup
dphys-swapfile swapon
echo 'vm.vfs_cache_pressure=50' >> /etc/sysctl.conf

## Dash
dpkg-reconfigure dash

## Preload
apt-get install -y preload
sed -i 's/sortstrategy = 3/sortstrategy = 0/g'  /etc/preload.conf

## IPV6
echo "net.ipv6.conf.all.disable_ipv6=1" > /etc/sysctl.d/disableipv6.conf
echo 'blacklist ipv6' >> /etc/modprobe.d/blacklist
sed -i '/::/s%^%#%g' /etc/hosts

## Update
wget http://goo.gl/1BOfJ -O /usr/bin/rpi-update && chmod +x /usr/bin/rpi-update

## Readahead
aptitude install readahead
touch /etc/readahead/profile-once
