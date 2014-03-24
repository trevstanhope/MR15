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
MONITOR_PARAMS = ['kill','lock','ignition','fuel','wheel']

# Commands
KILL = 0
STANDBY = 1
IGNITION = 2
BALLAST_UP = 3
BALLAST_DOWN = 4
STEERING_UP = 5
STEERING_DOWN = 6
WAIT = 255

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
            
        ## States
        self.state = 'off'
        self.steering_speed = 0
        self.ballast_speed = 0

    def get_current(self):
        try:
            json = self.monitor.readline()
            state = ast.literal_eval(json)
            for key in MONITOR_PARAMS:
                try:
                    state[key]
                except Exception:
                    return None
            return state
        except Exception as error:
            print str(error)
    
    def handle_fuel(self, current):
        if current:
            return current['fuel']
        else:
            return 0
        
    def handle_wheel(self, current):
        if current: 
            return current['wheel']
        else:
            return 0
        
    def handle_ballast(self, current):
        if current:
            ballast = current['ballast']
            if ballast == 1:
                action = BALLAST_UP
                self.ballast_speed += 1
            elif ballast == 0:
                action = WAIT
            elif ballast == -1:
                action = BALLAST_DOWN
                self.ballast_speed -= 1
        else:
            action = WAIT
        return action
        
    def handle_steering(self, current):
        if current:
            steering = current['steering']
            if steering == 1:
                action = STEERING_UP
                self.steering_speed += 1
            elif steering == 0:
                action = WAIT
            elif steering == -1:
                action = STEERING_DOWN
                self.steering_speed -= 1
        else:
            action = WAIT
        return action
    
    def handle_engine(self, current):
        if current:     
            if self.state == 'run':
                if current['kill'] == 1:
                    self.state = 'off'
                    action = KILL
                else:
                    action = WAIT
            elif self.state == 'standby':
                if current['ignition'] == 1:
                    self.state = 'run'
                    action = IGNITION
                else:
                    action = WAIT
            elif self.state == 'off':
                if current['rfid'] == 1:
                    self.state = 'standy'
                    action = STANDBY
                else:
                    action = WAIT
        else:
            action = WAIT
        return action

    def send_action(self, action):
        try:
            response = self.controller.write(action)
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
        
        ## Monitor Label
        self.monitor_var = tk.StringVar()
        self.monitor_var.set('None')
        monitor_label = tk.Label(
            self.master,
            textvariable=self.monitor_var,
            font=("Helvetica", 24)
        )
        monitor_label.pack()
        monitor_label.place(x=200, y=60)
        
        ## Controller Label
        self.control_var = tk.StringVar()
        self.control_var.set('None')
        control_label = tk.Label(
            self.master,
            textvariable=self.control_var,
            font=("Helvetica", 24)
        )
        control_label.pack()
        control_label.place(x=200, y=100)

        ## State Label
        self.state_var = tk.StringVar()
        self.state_var.set('None')
        state_label = tk.Label(
            self.master,
            textvariable=self.state_var,
            font=("Helvetica", 24)
        )
        state_label.pack()
        state_label.place(x=20, y=100)
        
        ## Fuel Label
        self.fuel_var = tk.StringVar()
        self.fuel_var.set('None')
        fuel_label = tk.Label(
            self.master,
            textvariable=self.fuel_var,
            font=("Helvetica", 24)
        )
        fuel_label.pack()
        fuel_label.place(x=20, y=140)
        
        ## Wheel Label
        self.wheel_var = tk.StringVar()
        self.wheel_var.set('None')
        wheel_label = tk.Label(
            self.master,
            textvariable=self.wheel_var,
            font=("Helvetica", 24)
        )
        wheel_label.pack()
        wheel_label.place(x=20, y=180)
        
        ## Ballast Label
        self.ballast_var = tk.StringVar()
        self.ballast_var.set('None')
        ballast_label = tk.Label(
            self.master,
            textvariable=self.ballast_var,
            font=("Helvetica", 24)
        )
        ballast_label.pack()
        ballast_label.place(x=20, y=220)
        
        ## Steering Label
        self.steering_var = tk.StringVar()
        self.steering_var.set('None')
        steering_label = tk.Label(
            self.master,
            textvariable=self.steering_var,
            font=("Helvetica", 24)
        )
        steering_label.pack()
        steering_label.place(x=20, y=260)

    def update(self, state='', control='', monitor='', error='', fuel='', wheel='', steering='', ballast=''):
        self.error_var.set('Errors: ' + str(error))
        self.monitor_var.set('Monitor Output: ' + str(monitor))
        self.control_var.set('Controller Action: ' + str(control))
        self.state_var.set('State: ' + str(state))
        self.fuel_var.set('Fuel Rate: ' + str(fuel))
        self.wheel_var.set('Wheel Rate: ' + str(wheel))
        self.steering_var.set('Steering Speed: ' + str(steering))
        self.ballast_var.set('Ballast Speed: ' + str(steering))
        self.master.update_idletasks()

# Main Loop
if __name__ == '__main__':
    display = Display(tk.Tk())
    tractor = Tractor()
    while True:
        try:
            current = tractor.get_current()
            engine_action = tractor.handle_engine(current)
            ballast_action = tractor.handle_ballast(current)
            steering_action = tractor.handle_steering(current)
            fuel_rate = tractor.handle_fuel(current)
            wheel_rate = tractor.handle_wheel(current)
            response = tractor.send_action(engine_action)
            display.update(
                error=response,
                monitor=current,
                control=engine_action,
                state=tractor.state,
                fuel=fuel_rate,
                wheel=wheel_rate,
                ballast=tractor.ballast_speed,
                steering=tractor.steering_speed
            )
        except KeyboardInterrupt:
            break
