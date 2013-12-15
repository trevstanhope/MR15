#!/usr/bin/env python

"""
Workstation System for the MR15
Developed by Macdonald Campus ASABE Tractor Pull Team
"""

# Modules
import ast
import serial
import Tkinter as tk
import time

# Global
MONITOR_DEV = '/dev/ttyACM0'
CONTROLLER_DEV = '/dev/ttyACM1'
BAUD = 115200

class MR15:
  
  ## Initialize MR15 systems
  def __init__(self):
    print('[Initializing PLCs]')
    try:
      self.ard_monitor = serial.Serial(MONITOR_DEV, BAUD)
      self.ard_controller = serial.Serial(CONTROLLER_DEV, BAUD)
      self.runlevel = 0
    except Exception as error:
      print('--> ' + str(error))
  
  ## 
  def monitor(self):
    print('[Monitoring Tractor]')
    try:
      json = self.ard_monitor.readline()
      sensors = ast.literal_eval(json)
      error = None
      print('--> ' + str(json))
    except Exception as error:
      sensors = {}
      print('--> ' + str(error))
    return (sensors, error)
    
  def decide(self, sensors):
    print('[Deciding Action]')
    if sensors:
      print('--> Working')
      actions = 1
    else:
      print('--> Nothing')
      actions = 0
    return actions
  
  def control(self, actions):
    print('[Controlling Tractor]')
    try:
      self.ard_controller.write(actions)
      error = None
    except Exception as error:
      print('--> ' + str(error))
    return error
      
class Display(object):

  ## Initialize the GUI
  def __init__(self, master, **kwargs):
    print('[Initializing Display]')
    pad = 3
    self.master = master
    self._geom='640x480+0+0'
    master.geometry("{0}x{1}+0+0".format(master.winfo_screenwidth()-pad, master.winfo_screenheight()-pad))
    self.set_layout() # call the set the UI layout fnc
  
  ## Set the layout of the GUI          
  def set_layout(self):
    print('[Setting Layout]')
    #self.master.overrideredirect(True) # make fullscreen
    self.master.focus_set()
    self.master.state("normal")
    ### Error Label
    self.error_var = tk.StringVar()
    self.error_var.set('None')
    error_label = tk.Label(self.master, textvariable=self.error_var, font=("Helvetica", 24))
    error_label.pack()
    error_label.place(x=20, y=20)
    ### Loop-speed Label
    self.speed_var = tk.StringVar()
    self.error_var.set('0')
    speed_label = tk.Label(self.master, textvariable=self.speed_var, font=("Helvetica", 24))
    speed_label.pack()
    speed_label.place(x=20, y=100)
    ### Sensors Label
    self.sensors_var = tk.StringVar()
    self.sensors_var.set('0')
    sensors_label = tk.Label(self.master, textvariable=self.sensors_var, font=("Helvetica", 24))
    sensors_label.pack()
    sensors_label.place(x=20, y=180)
  
  ## Update the error display
  def update_error(self, error):
    self.error_var.set(str(error))
    self.master.update_idletasks()
  
  ## Update the speed display
  def update_speed(self, speed):
    self.speed_var.set(str(speed))
    self.master.update_idletasks()

  ## Update the sensors display  
  def update_sensors(self, speed):
    self.sensors_var.set(str(speed))
    self.master.update_idletasks()
    
if __name__ == '__main__':

  ## Initialize objects
  app = Display(tk.Tk())
  tractor = MR15()
  
  ## Operation loop
  while True:
    a = time.time()
    
    (sensors, error) = tractor.monitor()
    app.update_sensors(sensors)
    app.update_error(error)
    
    actions = tractor.decide(sensors)
    
    error = tractor.control(actions)
    app.update_error(error)
    
    b = time.time()
    app.update_speed((b-a))
