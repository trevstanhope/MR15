# Arduino Makefile
# 1. Place this file in the directory with the .ino:
# 2. TARGET is the name of the .ino file
# 3. BOARD_TAG is the model of Arduino, usually 'uno'
# 4. PORT is the serial port, usually '/dev/ttyS0' or '/dev/ttyACM0'
# 5. ARDUINO_LIBS are the included libraries
#	e.g. ARDUINO_LIBS = SoftwareSerial Servo	
# 6.. Compile the file to Arduino
# 	make clean
#	make
#	make upload

ARDUINO_DIR = /usr/share/arduino
TARGET = monitor
BOARD_TAG = uno # alamode
ARDUINO_PORT = /dev/ttyACM0 # /dev/ttyS0
ARDUINO_LIBS = DHT stdio DallasTemperature OneWire
include /usr/share/arduino/Arduino.mk
