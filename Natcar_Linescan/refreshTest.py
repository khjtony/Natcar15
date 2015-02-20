import numpy as np
import matplotlib.pyplot as plt
import serial
import time
import threading

import Queue
from struct import unpack

x = np.linspace(1,128,128)
y = np.zeros(128)

# You probably won't need this if you're embedding things in a tkinter plot...
plt.ion()

fig = plt.figure()
ax = fig.add_subplot(111)
ax.set_ylim([0,255])
line1, = ax.plot(x, y, 'bo') # Returns a tuple of line objects, thus the comma


stargate = serial.Serial("COM10", 115200, timeout=1)
stargate.bytesize = serial.EIGHTBITS
stargate.parity = serial.PARITY_NONE
stargate.stopbits = serial.STOPBITS_ONE
dataQueue = []

i = 0
line1.set_xdata(np.arange(128))
line1.set_ydata(np.arange(128))
fig.canvas.draw()


def sample():
    global dataQueue
    if len(dataQueue) < 200:
        readResult = stargate.read(400)
        if readResult is None:
            pass
        elif len(readResult) is 0:
            pass
        else:
            dataQueue = dataQueue+list(readResult)
        # print dataQueue
        # print ":".join("{:02x}".format(ord(c)) for c in readResult)
        # print ord(dataQueue[i])
    # stop_index = 0
    PacketHeadFound = False
    for i in range(len(dataQueue)):
        # if len(dataQueue)<150:
        #     break
        if ord(dataQueue[i]) is 0xff and ord(dataQueue[i+1]) is 0x00 and ord(dataQueue[i+2]) is 0xff:
            # print "length of dataQueue = %s" % len(dataQueue)
            # print "Length of packet = %s" % len([ord(item) for item in dataQueue[i+3:i+3+128]])
            # ydata = [ord(item) for item in dataQueue[i+3:i+3+128]]
            # print ydata
            ydata = range(0, 128)
            line1.set_ydata(ydata)
            # line1.set_xdata(np.arange(128))
            # line1.set_ydata(range(1,129))

            # time.sleep(1) # delays for 5 seconds
            dataQueue = dataQueue[i+3+128:]
            i = 0
            PacketHeadFound = True
            break
        else:
            i += 1
    if PacketHeadFound:
        PacketHeadFound = False
    else:
        dataQueue = []
    fig.canvas.draw()
    threading.Timer(0.1, sample()).start()

sample()