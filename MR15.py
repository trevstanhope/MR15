#!/usr/bin/env python

"""
Tractor Performance System (TPS) for the MR15
Developed by Macdonald Campus ASABE Tractor Pull Team
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

# Keys
KILLS = ['button','seat','hitch']
LOCKS = ['cvt','brakes','rfid']
SENSORS = ['fuel','wheel']

# Commands
OFF = 0
STANDBY = 1
IGNITION = 2
WAIT = 255

# Empty Monitor String
with open('monitor.json') as sample:
    EMPTY = json.loads(sample.read())

# Control system class
class Tractor:

    def __init__(self):
        try:
            self.monitor = serial.Serial(MONITOR_DEV,MONITOR_BAUD)
        except Exception as error:
            print str(error)
        try:
            self.controller = serial.Serial(CONTROLLER_DEV,CONTROLLER_BAUD)
        except Exception as error:
            print str(error)
        self.state = 'off'

    def get_current(self):
        try:
            json = self.monitor.readline()
            state = ast.literal_eval(json)
            return state
        except Exception as error:
            print str(error)
    
    def get_state(self):
        return self.state

    def handle_state(self, current):
        current = EMPTY
        action = WAIT
        if self.state == 'run':
            if current['seat'] == 1:
                self.state = 'off'
                action = OFF
        elif self.state == 'standby':
            if current['ignition'] == 1:
                self.state = 'run'
                action = IGNITION
        elif self.state == 'off':
            if current['rfid'] == 1:
                self.state = 'standy'
                action = STANDBY
        return action

    def send_action(self, action):
        try:
            self.controller.write(actions)
            return response
        except Exception as error:
            print str(error) 

# Display system 
class Display(object):
    def __init__(self, master, **kwargs):
        pad = 3
        self.master = master
        self._geom='640x480+0+0'
        master.geometry("{0}x{1}+0+0".format(
            master.winfo_screenwidth()-pad,
            master.winfo_screenheight()-pad)
        )
        self.set_layout() # call the set the UI layout fnc
    
    def set_layout(self):
#        self.master.overrideredirect(True) # make fullscreen
        self.master.focus_set()
        self.master.state("normal")
        
        ## Error Label
        self.error_var = tk.StringVar()
        self.error_var.set('None')
        error_label = tk.Label(
            self.master,
            textvariable=self.error_var,
            font=("Helvetica", 24)
        )
        error_label.pack()
        error_label.place(x=20, y=20)

        ## State Label
        self.state_var = tk.StringVar()
        self.state_var.set('None')
        state_label = tk.Label(
            self.master,
            textvariable=self.state_var,
            font=("Helvetica", 24)
        )
        state_label.pack()
        state_label.place(x=20, y=80)

    def update(self, state, error):
        self.error_var.set(str(error))
        self.state_var.set(str(state))
        self.master.update_idletasks()

# Main Loop
if __name__ == '__main__':
    display = Display(tk.Tk())
    tractor = Tractor()
    while True:
        try:
            state = tractor.get_state()
            current = tractor.get_current()
            actions = tractor.handle_state(current)
            error = tractor.send_action(actions)
            display.update(current, error)
        except KeyboardInterrupt:
            break
