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
    # Create a UDP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Bind the socket to the port
    server_address = ('192.168.0.100', 1234)
    print('starting up on %s port %s' % server_address)
    sock.bind(server_address)

    bgg = BackgroundGenerator(debug=True)
    hd = HumanDetector(debug=True)

    cv2.namedWindow("window", cv2.WND_PROP_FULLSCREEN)
    cv2.setWindowProperty("window",cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)
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
        cv2.imshow('window', frame/255.0)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
