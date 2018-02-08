    #!/usr/bin/env python
    # -*- coding: utf-8 -*-

import cv2
import copy
import math
import matplotlib.pyplot as plt
import numpy as np
# network modules
import socket
import sys
from struct import *
# background substraction modules
from backgroundSubtraction.BackgroundGenerator import BackgroundGenerator
from backgroundSubtraction.HumanDetector import HumanDetector


if __name__ == '__main__':
    #Initiate
    initialized = False
    # Create a UDP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Bind the socket to the port
    server_address = ('192.168.0.100', 1234)
    print('starting up on %s port %s' % server_address)
    sock.bind(server_address)
    # create 4 subplots
    fig = plt.figure()
    ax1 = plt.subplot(2,2,1)
    ax2 = plt.subplot(2,2,2)
    ax3 = plt.subplot(2,2,3)
    ax4 = plt.subplot(2,2,4)

    bgg = BackgroundGenerator(debug=True)
    hd = HumanDetector(debug=True)
    
    plt.ion()
    while(True):
        # read a frame, frame is (60, 80, 3) unit8 array
        print('\nwaiting to receive message')
        data, address = sock.recvfrom(4800)
        
        print('received %s bytes from %s' % (len(data), address))
        img_tuple = unpack('B'*4800, data)
        print('unpacked %s bytes from data' % (len(img_tuple)))
        frame = np.array(img_tuple).reshape((60,80))
        frame = np.uint8(frame)
        median, binary, ccl, bg = bgg.apply(frame)
        diff, combined_diff, frame = hd.apply(frame, bg)
        if not initialized:
            im1 = ax1.imshow(frame)
            im2 = ax2.imshow(frame)
            im3 = ax3.imshow(frame)
            im4 = ax4.imshow(frame)
        else:
            im1.set_data(bg)
            im2.set_data(diff)
            im3.set_data(combined_diff)
            im4.set_data(frame)
        plt.pause(0.0001)
    plt.ioff() # due to infinite loop, this gets never called.
    plt.show()
