# MR15 Electronics System
## Overview
The MR15 features a dynamic new electronic system based on three independent components:
the Electronic Control Unit (ECU), Electronic Monitoring Unit (EMU) and
Vehicle Performace System (VPS).

### Electronic Control Unit (ECU)
The ECU is a dedicated ATmega1280 microcontroller for managing basic vehicle functionality.
Its responsibilities include managing several key sub-systems, specifically 
the Power Steering System (PSS), Electronic Ballast Ssystem (EBS),
Electronic Engine System (EES), and Fail-Safe System (FSS).

#### Power Steering Module (PSS)
Handles the electronic steering.

Features:
* 20 Amp Linear Actuator Controller
* Position feedback potentiometer
* Steering sensitivity control buttons

#### Electronic Ballast Module (EBS)
Controls the electronic ballast for in-pull weight management.

Features:
* 15 Amp Motor Controller
* Limit switches
* Ballast position control buttons

#### Electronic Engine Module (EES)
Handles the engine state.

Features:
* RFID key authentication
* Push-to-Start Ignition
* 3-channel relay module

#### Fail-Safe Module (FSS)
Integrated system which handles all vehicle safety features. Real-time detection
of any safety triggers and error checking allow the vehicle to reliably handle
emergency situations.
 
Features:
* Seat Killswitch
* Hitch Killswitch
* Button Killswitch
* CVT Guard Photosensor Engine Lock
* Dual brakes Engine Lock

### Electronic Monitoring Unit (EMU)
The EMU is a secondary ATMEL328 microcontroller which monitors the vehicle's embedded sensors.

Sensors:
* Internal Computer Conditions with a DHT22 Temperature/Humidity Sensor
* Fuel Flow Rate Sensor
* Wheel Speed Induction Sensor
* DS18B20 Engine Temperature Sensor

### Vehicle Performance System (VPS)
The VPS is a Linux nano-computer running on 700 MHz ARM processor which functions
as the both the Human-Machine Interface (HMI) and the Remote Diagnostic System (RDS).
Both the ECU and the EMU are directly interfaced with the VPS via UART serial interfaces.

Features:
* 802.11n Wifi Connectivity
* 7" TFT LCD and Real-time Graphical Disp

## Installation
Just run the following from your $HOME directory:

    git clone https://github.com/mutrac/MR15
    cd MR15
    chmod +x install.sh
    ./install.sh
   
## Configuration
### Configuring Arduinos (IMPORTANT)
The ECU and EMU subsystems must be upload to their respective Programmable Logic Controller (PLC).

To do so, connect the PLC and navigate to the proper folder (monitor/ or controller/).
Next, clean the directory of any previous build information:

    make clean
    
Then, compile the source code:

    make
    
Lastly, upload the compiled to the PLC:

    make upload

### Configuring Boot to Fullscreen (IMPORTANT)
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
    @python ~/MR15/MR15.py
    
### System Speed Tweak (Optional)
Open the system control file:

    sudo nano /etc/sysctl.conf
    
And add the following lines:
    
    vm.dirty_background_ratio = 20
    vm.dirty_expire_centisecs = 0
    vm.dirty_ratio = 80
    vm.dirty_writeback_centisecs = 1200
    vm.overcommit_ratio = 2
    vm.laptop_mode = 5
    vm.swappiness = 10

### Disable Getty (Optional)
Disable the unused terminals
    
    sed -i '/[2-6]:23:respawn:\/sbin\/getty 38400 tty[2-6]/s%^%#%g' /etc/inittab

### Disable Broadcom (Optional)
You can optionally disable Broadcom

    sudo modprobe -r snd-bcm2835

### Increase Swap (Optional)
Can increase the amount of swap (i.e. SD use)

    echo "CONF_SWAPSIZE=512" > /etc/dphys-swapfile
    dphys-swapfile setup
    dphys-swapfile swapon
    echo 'vm.vfs_cache_pressure=50' >> /etc/sysctl.conf

### Bash to Dash (Optional)
Enable dash by disabling bash

    dpkg-reconfigure dash

### Preload (Optional)
Preload loads things ahead of time, not as effective as it sounds.
    
    apt-get install -y preload
    sed -i 's/sortstrategy = 3/sortstrategy = 0/g'  /etc/preload.conf

### IPV6 (Optional)
IPV6 not needed, IPV4 can be kept during testing.

    echo "net.ipv6.conf.all.disable_ipv6=1" > /etc/sysctl.d/disableipv6.conf
    echo 'blacklist ipv6' >> /etc/modprobe.d/blacklist
    sed -i '/::/s%^%#%g' /etc/hosts
