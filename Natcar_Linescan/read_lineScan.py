__author__ = 'khjtonyX1'

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
from struct import unpack


fig = plt.figure()
ax = plt.axes(xlim=(0, 128), ylim=(0, 400))
ax.grid()
xdata, ydata = [], []
xdata = range(1, 129)
line, = ax.plot([], [], 'ro')
line.set_xdata(xdata)
# port = raw_input("Please Input COM: ")
port = 'com5'
car = serial.Serial(port.upper(), 115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, timeout=1)
one_line=np.zeros(128)
# initialization function: plot the background of each frame


def init():
    line.set_data([], [])
    return line,


def scan_line(gate):
    while (True):
        item = car.read()
        if (item=='\n'):
            continue
        else:
            one_line[0]=((ord(item)-ord('0'))*16)
            one_line[0]+=ord(car.read())-ord('0')
            for i in range(1,128):
              one_line[i]=((ord(car.read())-ord('0'))*16)
              one_line[i]+=ord(car.read())-ord('0')
            # print line
            car.flushInput()
            return one_line


def update_bar(data):
    global car
    # t, y = data
    # xdata.append(t)
    # ydata.append(y)
    # xmin, xmax = ax.get_xlim()
    #
    # line.set_data(np.linspace(1, 128, 128), scan_line(car))
    y_values = scan_line(car)
    # print y_values
    # for
    line.set_ydata(y_values)
    return line,


ani = animation.FuncAnimation(fig, update_bar, interval=100, blit=False)
plt.show()

# while(True):
#     scan_line(car)












