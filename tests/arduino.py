import time
import serial

ARDUINO_DEV = raw_input('Enter device path of arduino: ')
BAUD = int(raw_input('Enter the baud rate: '))
arduino = serial.Serial(ARDUINO_DEV, BAUD)

while True:
	a = time.time()
	print(arduino.readline())
	b = time.time()
	print(b - a)
