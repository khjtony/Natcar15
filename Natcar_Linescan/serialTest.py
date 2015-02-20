__author__ = 'khjtonyX1'
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
from struct import unpack
# port = raw_input("Please Input COM: ")
car = serial.Serial('COM10', 115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, timeout=1)
count=0
new_line=np.zeros(128)

fig = plt.figure()
ax = plt.axes(xlim=(0, 128), ylim=(0, 400))
xdata=[]
xdata = range(1, 129)
# line, = plt.plot([], [], 'ro')
line, =plt.plot(new_line)
plt.ion()
plt.show()

while(True):

    item=car.read()
    if (item=='\n'):
        continue
    else:
        # line[0]=((ord(item)-ord('0'))*16)
        # line[0]+=ord(car.read())-ord('0')
        # for i in range(1,128):
        #     line[i]=((ord(car.read())-ord('0'))*16)
        #     line[i]+=ord(car.read())-ord('0')
        # print line
        # car.flushInput()
        new_line[0]=((ord(item)-ord('0'))*16)
        new_line[0]+=ord(car.read())-ord('0')
        for i in range(1,128):
            new_line[i]=((ord(car.read())-ord('0'))*16)
            new_line[i]+=ord(car.read())-ord('0')
        # print line
        line.set_xdata(np.arange(len(new_line)))
        line.set_ydata(new_line)
        plt.draw()
        car.flushInput()



