#!/usr/bin/env python

"""
Tractor Performance System (TPS) for the MR15
Developed by Macdonald Campus ASABE Tractor Pull Team

TODO:
    - Add speedometer for wheel rpm
    - Add gauge for fuel rate
"""

# Modules
import ast
import json
import serial
import Tkinter as tk
import time

# Global
MONITOR_DEV = '/dev/ttyACM0'
CONTROLLER_DEV = '/dev/ttyACM1'
MONITOR_BAUD = 9600
CONTROLLER_BAUD = 9600
MONITOR_PARAMS = ['fuel','wheel']

# Commands
KILL = 'A'
STANDBY = 'B'
IGNITION = 'C'
BALLAST_UP = 'D'
BALLAST_DOWN = 'E'
STEERING_UP = 'F'
STEERING_DOWN = 'G'
WAIT = 'H'

# Control system class
class Tractor:

    def __init__(self):
        print('[Enabling Monitor]')
        try:
            self.monitor = serial.Serial(MONITOR_DEV,MONITOR_BAUD)
        except Exception as error:
            print('--> ' + str(error))
        print('[Enabling Controller]')
        try:
            self.controller = serial.Serial(CONTROLLER_DEV,CONTROLLER_BAUD)
        except Exception as error:
            print('--> ' + str(error))

    def get_sensors(self):
        print('[Getting Current Sensors]')
        try:
            literal = self.monitor.readline()
            sensors = ast.literal_eval(literal)
            # Test to make sure the object is complete
            for key in MONITOR_PARAMS:
                try:
                    sensors[key]
                except Exception:
                    return None
            return sensors
        except Exception as error:
            print('--> ' + str(error))
            
    def best(self, current, action=WAIT):
        print('[Deciding Best Action]')
        if current:
            fuel = current['fuel']
            wheel = current['wheel']
            action = WAIT
        print('--> ' + action)
        return action

    def send_action(self, action):
        print('[Sending Action]')
        try:
            response = self.controller.write(action)
            literal = self.controller.readline()
            response = ast.literal_eval(literal)
            print('--> ' + action)
            return response
        except Exception as error:
            print('--> ' + str(error))

# Display system 
class Display(object):
    def __init__(self, master, **kwargs):
        print('[Initializing Display]')
        pad = 3
        self.master = master
        self._geom='640x480+0+0'
        master.geometry("{0}x{1}+0+0".format(
            master.winfo_screenwidth()-pad,
            master.winfo_screenheight()-pad)
        )
        self.set_layout() # call the set the UI layout fnc
    
    def set_layout(self):
        print('[Setting Layout]')
#        self.master.overrideredirect(True) # make fullscreen
        self.master.focus_set()
        self.master.state("normal")
        
        ## Fuel Label
        self.fuel_var = tk.StringVar()
        self.fuel_var.set('Fuel Rate: ?')
        fuel_label = tk.Label(
            self.master,
            textvariable=self.fuel_var,
            font=("Helvetica", 24)
        )
        fuel_label.pack()
        fuel_label.place(x=20, y=140)
        
        ## Wheel Label
        self.wheel_var = tk.StringVar()
        self.wheel_var.set('Wheel Rate: ?')
        wheel_label = tk.Label(
            self.master,
            textvariable=self.wheel_var,
            font=("Helvetica", 24)
        )
        wheel_label.pack()
        wheel_label.place(x=20, y=180)
        
        ## Frequency Label
        self.freq_var = tk.StringVar()
        self.freq_var.set('Update Rate: ?')
        freq_label = tk.Label(
            self.master,
            textvariable=self.freq_var,
            font=("Helvetica", 24)
        )
        freq_label.pack()
        freq_label.place(x=20, y=220)
        
        ## Brakes Label
        self.brakes_var = tk.StringVar()
        self.brakes_var.set('Brakes: ?')
        brakes_label = tk.Label(
            self.master,
            textvariable=self.brakes_var,
            font=("Helvetica", 24)
        )
        brakes_label.pack()
        brakes_label.place(x=20, y=260)
        
        ## Guard Label
        self.guard_var = tk.StringVar()
        self.guard_var.set('CVT Guard: ?')
        guard_label = tk.Label(
            self.master,
            textvariable=self.guard_var,
            font=("Helvetica", 24)
        )
        guard_label.pack()
        guard_label.place(x=20, y=300)

    def update(self, monitor,control,freq):
        print('[Updating Display]')
        if (monitor and control):
            self.fuel_var.set('Fuel Rate: ' + str(monitor['fuel']))
            self.wheel_var.set('Wheel Rate: ' + str(monitor['wheel']))
            self.brakes_var.set('Brakes: ' + str(control['brakes']))
            self.guard_var.set('CVT Guard: ' + str(control['guard']))
        self.freq_var.set('Update Rate: ' + str(freq))
        self.master.update_idletasks()

# Main Loop
if __name__ == '__main__':
    root = tk.Tk()
    root.config(background = "#FFFFFF") #sets background color to white
    display = Display(root)
    tractor = Tractor()
    while True:
        try:
            a = time.time()
            sensors = tractor.get_sensors()
            action = tractor.best(sensors)
            response = tractor.send_action(action)
            b = time.time()
            freq = (b-a)
            display.update(sensors, response, freq)
        except KeyboardInterrupt:
            break
