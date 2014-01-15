# Mutrac MR15

## Installation
Just run the following from your $HOME directory:

    git clone https://github.com/mutrac/MR15
    cd MR15
    chmod +x install.sh
    ./install.sh
   
## Configuration
### Configuring Arduinos (IMPORTANT)
The Controller and Monitor subsystems must be loaded to their respective PLC.
Please check the README.md

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
    @python ~/MR15/examples/fullscreen_tkinter.py # will change later to MR15.py
    
### System Speed Tweak (Optional)
sudo nano /etc/sysctl.conf
    
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
