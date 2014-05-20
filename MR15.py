#!/usr/bin/env python

"""
Vehicle Performance System (VPS) for the MR15
Developed by Macdonald Campus ASABE Tractor Pull Team

TODO:
    - Improve GUI
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
MONITOR_DEV = '/dev/ttyS0' # '/dev/ttyS0'
CONTROLLER_DEV = '/dev/ttyACM0' # '/dev/ttyACM0'
MONITOR_BAUD = 9600
CONTROLLER_BAUD = 9600
MONITOR_PARAMS = ['box_temp','box_rh', 'engine_lph','engine_rpm', 'engine_temp']
CONTROLLER_PARAMS = ['brakes','seat','hitch','guard','near','far', 'state','ignition']
SERIAL_TIMEOUT = 0.1

# Control system class
class Tractor:
    
    def __init__(self):
        print('[Enabling Monitor]')
        try:
            self.monitor = serial.Serial(MONITOR_DEV, MONITOR_BAUD, timeout=SERIAL_TIMEOUT)
        except Exception as error:
            print('\t' + str(error))
        print('[Enabling Controller]')
        try:
            self.controller = serial.Serial(CONTROLLER_DEV, CONTROLLER_BAUD,timeout=SERIAL_TIMEOUT)
        except Exception as error:
            print('\t' + str(error))
            
    def check_monitor(self):
        print('[Getting EMU State]')
        try:
            literal = self.monitor.readline()
            print('\tBuffer: ' + str(literal))
            response = ast.literal_eval(literal)
            for key in MONITOR_PARAMS:
                try:
                    response[key]
                except Exception:
                    response[key] = None
            print('\tParsed: ' + str(response))
            return response
        except Exception as error:
            print('\t' + str(error))
            
    def check_controller(self):
        print('[Getting ECU State]')
        try:
            literal = self.controller.readline()
            print('\tBuffer: ' + str(literal))
            response = ast.literal_eval(literal)
            for key in CONTROLLER_PARAMS:
                try:
                    response[key]
                except Exception:
                    response[key] = None
            print('\tParsed: ' + str(response))
            return response
        except Exception as error:
            print('\t' + str(error))

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
        self.master.overrideredirect(True) # make fullscreen
        self.master.focus_set()
        self.master.state("normal")
        
        ## EMU Label
        self.emu_var = tk.StringVar()
        self.emu_var.set('EMU')
        emu_label = tk.Label(
            self.master,
            textvariable=self.emu_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        emu_label.pack()
        emu_label.place(x=20, y=0)
        
        ## Fuel Label
        self.fuel_var = tk.StringVar()
        self.fuel_var.set('Fuel Rate (L/H): ?')
        fuel_label = tk.Label(
            self.master,
            textvariable=self.fuel_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        fuel_label.pack()
        fuel_label.place(x=20, y=40)
        
        ## Wheel Label
        self.wheel_var = tk.StringVar()
        self.wheel_var.set('Engine Rate (RPM): ?')
        wheel_label = tk.Label(
            self.master,
            textvariable=self.wheel_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        wheel_label.pack()
        wheel_label.place(x=20, y=80)
        
        ## Temperature Label
        self.temp_var = tk.StringVar()
        self.temp_var.set('Temperature (C): ?')
        temp_label = tk.Label(
            self.master,
            textvariable=self.temp_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        temp_label.pack()
        temp_label.place(x=20, y=120)
        
        ## Humidity Label
        self.humidity_var = tk.StringVar()
        self.humidity_var.set('Humidity (RH): ?')
        humidity_label = tk.Label(
            self.master,
            textvariable=self.humidity_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        humidity_label.pack()
        humidity_label.place(x=20, y=160)
        
        ## ECU Label
        self.ecu_var = tk.StringVar()
        self.ecu_var.set('ECU')
        ecu_label = tk.Label(
            self.master,
            textvariable=self.ecu_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        ecu_label.pack()
        ecu_label.place(x=20, y=200)
        
        ## State Label
        self.state_var = tk.StringVar()
        self.state_var.set('State: ?')
        state_label = tk.Label(
            self.master,
            textvariable=self.state_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        state_label.pack()
        state_label.place(x=20, y=240)
        
        ## Steering Sensitivity Label
        self.steering_var = tk.StringVar()
        self.steering_var.set('Steering Sensitivity: ?')
        steering_label = tk.Label(
            self.master,
            textvariable=self.steering_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        steering_label.pack()
        steering_label.place(x=20, y=280)
        
        ## Ballast Speed Label
        self.ballast_var = tk.StringVar()
        self.ballast_var.set('Ballast Speed: ?')
        ballast_label = tk.Label(
            self.master,
            textvariable=self.ballast_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        ballast_label.pack()
        ballast_label.place(x=20, y=320)
        
    def update(self, monitor, control):
        print('[Updating Display]')
        if (monitor):
            self.fuel_var.set('Fuel Rate (L/H): ' + str(monitor['engine_lph']))
            self.wheel_var.set('Engine Rate (RPM): ' + str(monitor['engine_rpm']))
            self.temp_var.set('Temperature (C): ' + str(monitor['box_temp']))
            self.humidity_var.set('Humidity (RH): ' + str(monitor['box_rh']))
        else:
            print('\t NO EMU DATA')
        if (control):
            try:
                if control['state'] == 0:
                    state = 'OFF'
                elif control['state'] == 1:
                    state = 'STANDBY'
                elif control['state'] == 2:
                    state = 'RUNNING'
                self.state_var.set('State: ' + state)
            except Exception:
                print('\t NO STATE FOUND')
            try:
                self.steering_var.set('Steering Sensitivity: ' + str(control['steering']))
            except Exception:
                print('\t NO STEERING SENSITIVITY FOUND')
            try:
                self.ballast_var.set('Ballast Speed: ' + str(control['ballast']))
            except Exception:
                print('\t NO BALLAST SPEED FOUND')
        else:
            print('\t NO ECU DATA')
        self.master.update_idletasks()

# Main Loop
if __name__ == '__main__':
    root = tk.Tk()
    root.config(background = "#FFFFFF") #sets background color to white
    display = Display(root)
    tractor = Tractor()
    while True:
        try:
            monitor = tractor.check_monitor()
            control = tractor.check_controller()
            display.update(monitor, control)
        except KeyboardInterrupt:
            break
