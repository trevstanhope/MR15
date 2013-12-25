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
BAUD = 9600

# Control system class
class Control:
  
  ## Initialize MR15 systems
  def __init__(self):

    ### Setup Monitor
    print('[Initializing Monitor PLC]')
    try:
      self.ard_monitor = serial.Serial(MONITOR_DEV, BAUD)
    except Exception as error:
      print('--> ' + str(error))

    ## Setup Controller
    print('[Initializing Control PLC]')
    try:
      self.ard_controller = serial.Serial(CONTROLLER_DEV, BAUD)      
    except Exception as error:
      print('--> ' + str(error))

    ## Setup States
    print('[Initializing States]')
    try:
      self.state = 0
      self.steering = 0
      self.ballast = 0
    except Exception as error:
      print('--> ' + str(error))
  
  ## Monitor
  def monitor(self):
    print('[Monitoring Tractor]')
    try:
      json = self.ard_monitor.readline()
      sensors = ast.literal_eval(json)
      for key in sensors:
        print('--> ' + key + ':' + str(sensors[key]))
    except Exception as error:
      print('--> ' + str(error))
      sensors = None
    return sensors

  ## Decide Action
  def decide(self, sensors):
    print('[Deciding Action]')
    if sensors:
      if (sensors['button'] or sensors['hitch'] or sensors['seat']) == 1:
        print('--> KILLED')
        action = 255
      elif sensors['rfid'] == 1:
        print('--> STANDBY')
        action = 1
      else:
        if sensors['ignition'] == 1:
          if (sensors['guard'] and sensors['brake']) == 0:
            print('--> IGNITION')
            action = 2
          else:
            print('--> LOCKED')
            action = 0
        else:
          print('--> NOT IGNITION')
          action = 2
    else:
      print('--> READ FAILURE')
      action = 0
    print('--> ACTION: ' + str(action))
    return action

  ## Control
  def control(self, actions):
    print('[Controlling Tractor]')
    try:
      self.ard_controller.write(actions)
      error = None
    except Exception as error:
      print('--> ' + str(error))
    return error

# Display system 
class Display(object):

  ## Initialize the GUI
  def __init__(self, master, **kwargs):

    ### Initialize Display
    print('[Initializing Display]')
    pad = 3
    self.master = master
    self._geom='640x480+0+0'
    master.geometry("{0}x{1}+0+0".format(master.winfo_screenwidth()-pad, master.winfo_screenheight()-pad))
    self.set_layout() # call the set the UI layout fnc
  
  ## Set the layout of the GUI          
  def set_layout(self):

    ### Initialize Layout
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

# Main Loop
if __name__ == '__main__':

  ## Initialize objects
  app = Display(tk.Tk())
  tractor = Control()
  
  ## Operation loop
  while True:
    a = time.time()
    sensors = tractor.monitor()
    actions = tractor.decide(sensors)
    error = tractor.control(actions)
    app.update_sensors(sensors)
    app.update_error(error)
    b = time.time()
    app.update_speed((b-a))
