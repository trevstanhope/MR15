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
1. Install FBI
    apt-get install fbi
2. Copy your custom splash image to /etc/ and name it "splash.png".
3. Next, copy the init.d script called "asplashscreen" from "config/" into "/etc/init.d/".
4. Make it executable
    chmod a+x /etc/init.d/asplashscreen
5. Enable it as an runtime script
    insserv /etc/init.d/asplashscreen
6. Reboot
    reboot
