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

class IRCamera(object):
    def __init__(self):
        # Create a UDP/IP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Bind the socket to the port
        server_address = ('192.168.0.100', 1234)
        print('starting up on %s port %s' % server_address)
        self.sock.bind(server_address)

        self.bgg = BackgroundGenerator(debug=True)
        self.hd = HumanDetector(debug=True)
    
    def __del__(self):
        self.sock.close()
    
    def get_frame(self):
        print('\nwaiting to receive message')
        data, address = self.sock.recvfrom(4800)
        
        print('received %s bytes from %s' % (len(data), address))
        img_tuple = unpack('B'*4800, data)
        print('unpacked %s bytes from data' % (len(img_tuple)))
        frame = np.array(img_tuple).reshape((60,80))
        frame = np.uint8(frame)
        median, binary, ccl, bg = self.bgg.apply(frame)
        diff, combined_diff, frame = self.hd.apply(frame, bg)
        # 2d grey to 3d image
        img_bgr = cv2.cvtColor(frame, cv2.COLOR_GRAY2BGR)
        # convert 3d to 1d
        ret, jpeg = cv2.imencode('.jpg', img_bgr)
        return jpeg.tobytes()