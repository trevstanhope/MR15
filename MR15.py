#!/usr/bin/env python

"""
Vehicle Performance System (VPS) for the MR15
Developed by Macdonald Campus ASABE Tractor Pull Team

TODO:
    - Improve GUI
    - User Logging
"""

# Modules
import ast
import json
import serial
import Tkinter as tk
import time
from datetime import datetime

# Global
MONITOR_DEV = '/dev/ttyACM0' # '/dev/ttyS0'
CONTROLLER_DEV = '/dev/ttyACM1' # '/dev/ttyACM0'
MONITOR_BAUD = 9600
CONTROLLER_BAUD = 9600
SERIAL_TIMEOUT = 0.01

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
            print('\tParsing: OKAY')
            return response
        except Exception as error:
            print('\tERROR: ' + str(error))
            
    def check_controller(self):
        print('[Getting ECU State]')
        try:
            literal = self.controller.readline()
            print('\tBuffer: ' + str(literal))
            response = ast.literal_eval(literal)
            print('\tParsing: OKAY')
            return response
        except Exception as error:
            print('\tERROR: ' + str(error))

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
        
        ## Global Layout
        self.master.overrideredirect(True) # make fullscreen
        self.master.focus_set()
        self.master.state("normal")
        
        ## EMU Label
        self.emu_var = tk.StringVar()
        self.emu_var.set('EMU: Initializing')
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
        self.engine_temp_var = tk.StringVar()
        self.engine_temp_var.set('Engine Temperature (C): ?')
        engine_temp_label = tk.Label(
            self.master,
            textvariable=self.engine_temp_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        engine_temp_label.pack()
        engine_temp_label.place(x=20, y=120)
        
        ## ECU Label
        self.ecu_var = tk.StringVar()
        self.ecu_var.set('ECU: Initializing')
        ecu_label = tk.Label(
            self.master,
            textvariable=self.ecu_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        ecu_label.pack()
        ecu_label.place(x=20, y=200)
        
        ## Warnings Label
        self.warnings_var = tk.StringVar()
        self.warnings_var.set('Warnings: None')
        warnings_label = tk.Label(
            self.master,
            textvariable=self.warnings_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        warnings_label.pack()
        warnings_label.place(x=20, y=240)
        
        ## State Label
        self.state_var = tk.StringVar()
        self.state_var.set('Engine: ?')
        state_label = tk.Label(
            self.master,
            textvariable=self.state_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        state_label.pack()
        state_label.place(x=20, y=280)
        
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
        steering_label.place(x=20, y=320)
        
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
        ballast_label.place(x=20, y=360)
        
        ## Time Label
        self.time_var = tk.StringVar()
        self.time_var.set('Run Time: ?')
        time_label = tk.Label(
            self.master,
            textvariable=self.time_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        time_label.pack()
        time_label.place(x=480, y=0)
        
        ## Time Label
        self.user_var = tk.StringVar()
        self.user_var.set('User: ?')
        user_label = tk.Label(
            self.master,
            textvariable=self.user_var,
            font=("Helvetica", 24),
            bg="#FFFFFF"
        )
        user_label.pack()
        user_label.place(x=480, y=40)
        
    def update(self, tractor, monitor, control):
        print('[Updating Display]')
    
        ## Set EMU labels
        if (monitor):
            self.emu_var.set('EMU: Okay')
            try:
              self.fuel_var.set('Fuel Rate (L/H): %s' % str(monitor['engine_lph']))
              self.wheel_var.set('Engine Rate (RPM): %s' % str(monitor['engine_rpm']))
              self.engine_temp_var.set('Engine Temperature (C): %s' % str(monitor['engine_temp']))
            except Exception as err:
              pass
        else:
            self.emu_var.set('EMU: Waiting')
            
        ## Set ECU labels
        if (control):
            self.ecu_var.set('ECU: Okay')
            
            ### Kill Warnings
            try:
                if control['button']:
                    self.warnings_var('Warnings: Kill button pressed')
                elif control['hitch']:
                    self.warnings_var('Warnings: Hitch detached')
                elif control['seat']:
                    self.warnings_var('Warnings: No operator in seat')
                else:
                    self.warnings_var('Warnings: None')
            except Exception:
                pass
                
            ### Engine State
            try:
                if control['state'] == 0:
                    self.state_var.set('Engine: %s' % 'OFF')
                elif control['state'] == 1:
                    self.state_var.set('Engine: %s' % 'ENGAGED')
                    self.user_var.set('User: %s' % 'Trevor Stanhope')
                    if control['ignition'] and not control['guard']:
                        self.warnings_var('Warnings: CVT guard open')
                    elif control['ignition'] and not control['brakes']:
                        self.warnings_var('Warnings: Brakes not engaged')
                elif control['state'] == 2:
                    self.state_var.set('Engine: %s' % 'RUNNING')
            except Exception:
                pass
            
            ### Steering
            try:
                self.steering_var.set('Steering Sensitivity: %s' % str(control['str_spd']))
            except Exception:
                self.warning_var.set('Warnings: Failed to get sensitivity')
                
            ### Ballast
            try:
                self.ballast_var.set('Ballast Speed: %s' % str(control['bal_spd']))
            except Exception:
                self.warning_var.set('Warnings: Failed to get ballast speed')
        else:
            self.ecu_var.set('ECU: Waiting')
        
        ### Running Time
        self.time_var.set('On Time: %s' % str(datetime.now() - tractor.on_time))
            
        ## Update all changes
        self.master.update_idletasks()

# Main Loop
if __name__ == '__main__':

    ## Initialize Display
    try:
        root = tk.Tk()
        root.config(background = "#FFFFFF") # sets background color to white
        display = Display(root)
        GTK = True
    except Exception as err:
       print('NO GTK')
       GTK = False
       
    ## Run Tractor
    tractor = Tractor()
    tractor.on_time = datetime.now()
    while True:
        try:
            monitor = tractor.check_monitor()
            control = tractor.check_controller()
            if GTK: display.update(tractor, monitor, control)
        except KeyboardInterrupt:
            break
