#!/usr/bin/env python
import ast
import serial

MONITOR_DEV = '/dev/ttyAMC0'
CONTROLLER_DEV = '/dev/ttyAMC1'
BAUD = 9600

class MR15:

  def __init__(self):
    print('[Initializing PLCs]')
    try:
      self.ard_monitor = serial.Serial(MONITOR_DEV, BAUD)
      self.ard_controller = serial.Serial(CONTROLLER_DEV, BAUD)
    except Exception as error:
      print('--> ' + str(error))
    
  def monitor(self):
    print('[Monitoring Tractor]')
    try:
      json = self.ard_monitor.read()
      sensors = ast.literal_eval(json)
    except Exception as error:
      print('--> ' + str(error))
    return sensors
    
  def control(self, sensors):
    print('[Controlling Tractor]')
    try:
      self.ard_controller.write()
    except Exception as error:
      print('--> ' + str(error))
    
if __name__ == '__main__':
  try:
    tractor = MR15()
    while True:
      sensors = tractor.monitor()
      tractor.control(sensors)
  except Exception as error:
    print('--> ' + str(error))
  

