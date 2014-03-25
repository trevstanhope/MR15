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
MONITOR_PARAMS = ['lock','ignition','fuel','wheel','rfid']

# Commands
KILL = '0'
STANDBY = '1'
IGNITION = '2'
BALLAST_UP = '3'
BALLAST_DOWN = '4'
STEERING_UP = '5'
STEERING_DOWN = '6'
WAIT = '7'

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

    def get_current(self):
        print('[Getting Current Sensors]')
        try:
            json = self.monitor.readline()
            current = ast.literal_eval(json)
            # Test to make sure the object is complete
            for key in MONITOR_PARAMS:
                try:
                    state[key]
                except Exception:
                    return None
            return current
        except Exception as error:
            print('--> ' + str(error))
            
    def best(self, current, action=WAIT):
        print('[Deciding Best Action]')
        if current:
            rfid = current['rfid']
            lock = current['lock']
            ballast = current['ballast']
            ignition = current['rfid']
            steering = current['steering']
            if (rfid == 1):
                action = STANDBY
            elif (ignition == 1) and (lock == 0):
                action = IGNITION
            elif (steering == 1): 
                action = STEERING_UP
            elif (steering == -1): 
                action = STEERING_DOWN
            elif (ballast == 1):
                action == BALLAST_UP
            elif (ballast == -1):
                action == BALLAST_DOWN
        print('--> ' + action)
        return action

    def send_action(self, action):
        print('[Sending Action]')
        try:
            response = self.controller.write(action)
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

    def update(self, current, freq):
        print('[Updating Display]')
        if current:
            self.fuel_var.set('Fuel Rate: ' + str(current['fuel']))
            self.wheel_var.set('Wheel Rate: ' + str(current['wheel']))
            self.brakes_var.set('Brakes: ' + str(current['brakes']))
            self.guard_var.set('CVT Guard: ' + str(current['guard']))
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
            current = tractor.get_current()
            action = tractor.best(current)
            response = tractor.send_action(action)
            b = time.time()
            display.update(current, (b-a))
        except KeyboardInterrupt:
            break
