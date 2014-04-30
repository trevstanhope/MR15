from Tkinter import *
from PIL import Image, ImageTk
import cmath, math


class Speedometer():

    def __init__(self):
        self.root = Tk()
        self.totalkm = 0
        self.atobkm = 0
        self.atobActive = 0
        
        self.canvas = Canvas(self.root,width = 610,height = 610)
        self.canvas.grid(row = 0, column = 1,rowspan = 3)

        self.scaler = Scale(self.root, from_=270, to = 0,command = self.updateCursor)
        self.scaler.grid(row = 1, column = 0,sticky = "ns")

        self.atobButton = Button(text = "A to B",command = self.atobHandler,relief = FLAT)
        self.canvas.create_window(290, 530, anchor=NW, window=self.atobButton)

        bgimage = Image.open("speedometer.png")
        bgphoto = ImageTk.PhotoImage(bgimage)
        self.background = self.canvas.create_image(10, 10, anchor=NW, image=bgphoto)

        self.cursor = self.canvas.create_line(150,150,310,310,width = 5.0,fill="white")
        self.total_odometer = self.canvas.create_text(310,460,text = "Total:    0.0 km")
        self.atob_odometer = self.canvas.create_text(310,500,text = "A to B:   0.0 km")

        self.updateClock()
        self.root.title("Onur Dincol --- Efe Orak     ||     Speed-o-meter")
        self.mainloop()

    def atobHandler(self):
        if self.atobActive == 0:
            self.atobActive = 1
            self.canvas.itemconfig(self.atob_odometer,text = "A to B:   " + "0.0" + " km")
        else:
            self.atobkm = 0
            self.atobActive = 0
            
    def updateCursor(self,event):
        value = self.scaler.get() + 45
        value = value * 0.0174532925
        
        center = complex(310,310)
        cangle = cmath.exp(value*1j)
        v = cangle * (complex(310,540) - center) + center

        self.canvas.delete(self.cursor)
        self.cursor = self.canvas.create_line(v.real,v.imag,310,310,width = 5.0,fill="white")

    def mainloop(self):
        self.root.mainloop()

    def updateClock(self):
        speed = self.scaler.get() / 3600.0
        self.totalkm = self.totalkm + speed
        self.canvas.itemconfig(self.total_odometer,text = "Total:    " + str(self.totalkm) + " km")

        if(self.atobActive == 1):
            self.atobkm = self.atobkm + speed
            self.canvas.itemconfig(self.atob_odometer,text = "A to B:   "+ str(self.atobkm) + " km")
            
        self.root.after(1000, self.updateClock)

m = Speedometer()
