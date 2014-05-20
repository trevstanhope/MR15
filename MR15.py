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
MONITOR_DEV = '/dev/ttyACM1' # '/dev/ttyS0'
CONTROLLER_DEV = '/dev/ttyACM0' # '/dev/ttyACM0'
MONITOR_BAUD = 9600
CONTROLLER_BAUD = 9600
MONITOR_PARAMS = ['fuel','wheel', 'temp','humidity']
CONTROLLER_PARAMS = ['brakes','seat','hitch','guard','near','far', 'state','ignition']

# Control system class
class Tractor:
    
    def __init__(self):
        print('[Enabling Monitor]')
        try:
            self.monitor = serial.Serial(MONITOR_DEV,MONITOR_BAUD, timeout=0.1)
        except Exception as error:
            print('--> ' + str(error))
        print('[Enabling Controller]')
        try:
            self.controller = serial.Serial(CONTROLLER_DEV,CONTROLLER_BAUD,timeout=0.1)
        except Exception as error:
            print('--> ' + str(error))
            
    def check_monitor(self):
        print('[Getting EMU State]')
        try:
            literal = self.monitor.readline()
            response = ast.literal_eval(literal)
            for key in MONITOR_PARAMS:
                try:
                    response[key]
                except Exception:
                    response[key] = 0
            print('\t' + str(response))
            return response
        except Exception as error:
            print('--> ' + str(error))
            
    def check_controller(self):
        print('[Getting ECU State]')
        try:
            literal = self.controller.readline()
            response = ast.literal_eval(literal)
            for key in CONTROLLER_PARAMS:
                try:
                    response[key]
                except Exception:
                    response[key] = 0
            print('\t' + str(response))
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
        self.master.overrideredirect(True) # make fullscreen
        self.master.focus_set()
        self.master.state("normal")
        
        ## Fuel Label
        self.fuel_var = tk.StringVar()
        self.fuel_var.set('Fuel Rate (L/H): ?')
        fuel_label = tk.Label(
            self.master,
            textvariable=self.fuel_var,
            font=("Helvetica", 24)
        )
        fuel_label.pack()
        fuel_label.place(x=20, y=40)
        
        ## Wheel Label
        self.wheel_var = tk.StringVar()
        self.wheel_var.set('Wheel Rate (RPM): ?')
        wheel_label = tk.Label(
            self.master,
            textvariable=self.wheel_var,
            font=("Helvetica", 24)
        )
        wheel_label.pack()
        wheel_label.place(x=20, y=80)
        
        ## Temperature Label
        self.temp_var = tk.StringVar()
        self.temp_var.set('Temperature (C): ?')
        temp_label = tk.Label(
            self.master,
            textvariable=self.temp_var,
            font=("Helvetica", 24)
        )
        temp_label.pack()
        temp_label.place(x=20, y=120)
        
        ## Humidity Label
        self.humidity_var = tk.StringVar()
        self.humidity_var.set('Humidity (RH): ?')
        humidity_label = tk.Label(
            self.master,
            textvariable=self.humidity_var,
            font=("Helvetica", 24)
        )
        humidity_label.pack()
        humidity_label.place(x=20, y=160)
        
        ## Frequency Label
        self.freq_var = tk.StringVar()
        self.freq_var.set('Frequency (Hz): ?')
        freq_label = tk.Label(
            self.master,
            textvariable=self.freq_var,
            font=("Helvetica", 24)
        )
        freq_label.pack()
        freq_label.place(x=20, y=200)
        
        ## State Label
        self.state_var = tk.StringVar()
        self.state_var.set('State: ?')
        state_label = tk.Label(
            self.master,
            textvariable=self.state_var,
            font=("Helvetica", 24)
        )
        state_label.pack()
        state_label.place(x=20, y=240)
        
        ## Brakes Label
        self.brakes_var = tk.StringVar()
        self.brakes_var.set('Brakes: ?')
        brakes_label = tk.Label(
            self.master,
            textvariable=self.brakes_var,
            font=("Helvetica", 24)
        )
        brakes_label.pack()
        brakes_label.place(x=20, y=280)
        
        ## Guard Label
        self.guard_var = tk.StringVar()
        self.guard_var.set('CVT Guard: ?')
        guard_label = tk.Label(
            self.master,
            textvariable=self.guard_var,
            font=("Helvetica", 24)
        )
        guard_label.pack()
        guard_label.place(x=20, y=320)
        
        ## Seat Label
        self.seat_var = tk.StringVar()
        self.seat_var.set('Seat: ?')
        seat_label = tk.Label(
            self.master,
            textvariable=self.seat_var,
            font=("Helvetica", 24)
        )
        seat_label.pack()
        seat_label.place(x=20, y=360)
        
        ## Hitch Label
        self.hitch_var = tk.StringVar()
        self.hitch_var.set('Hitch: ?')
        hitch_label = tk.Label(
            self.master,
            textvariable=self.hitch_var,
            font=("Helvetica", 24)
        )
        hitch_label.pack()
        hitch_label.place(x=20, y=400)
        
        ## Steering Sensitivity Label
        self.steering_var = tk.StringVar()
        self.steering_var.set('Steering Sensitivity: ?')
        steering_label = tk.Label(
            self.master,
            textvariable=self.steering_var,
            font=("Helvetica", 24)
        )
        steering_label.pack()
        steering_label.place(x=20, y=440)
        
        ## Ballast Speed Label
        self.ballast_var = tk.StringVar()
        self.ballast_var.set('Ballast Speed: ?')
        ballast_label = tk.Label(
            self.master,
            textvariable=self.ballast_var,
            font=("Helvetica", 24)
        )
        ballast_label.pack()
        ballast_label.place(x=20, y=480)
        
    def update(self, monitor, control, freq):
        print('[Updating Display]')
        if (monitor):
            self.fuel_var.set('Fuel Rate (L/H): ' + str(monitor['fuel']))
            self.wheel_var.set('Wheel Rate (RPM): ' + str(monitor['wheel']))
            self.temp_var.set('Temperature (C): ' + str(monitor['temp']))
            self.humidity_var.set('Humidity (RH): ' + str(monitor['humidity']))
        if (control):
            self.state_var.set('State: ' + str(control['state']))
            self.brakes_var.set('Brakes: ' + str(control['brakes']))
            self.seat_var.set('Seat: ' + str(control['seat']))
            self.hitch_var.set('Hitch: ' + str(control['hitch']))
            self.guard_var.set('CVT Guard: ' + str(control['guard']))
            self.steering_var.set('Steering Sensitivity: ' + str(control['steering']))
            self.ballast_var.set('Ballast Speed: ' + str(control['ballast']))
        self.freq_var.set('Frequency (Hz): ' + str(freq))
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
            monitor = tractor.check_monitor()
            control = tractor.check_controller()
            b = time.time()
            freq = int(1/(b-a))
            display.update(monitor, control, freq)
        except KeyboardInterrupt:
            break
