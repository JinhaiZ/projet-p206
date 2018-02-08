#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket
import sys

from struct import *
import numpy as np
from matplotlib import pyplot as plt

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the port
server_address = ('192.168.0.100', 1234)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
squence = 0
fig = plt.figure()
ax1 = plt.subplot(1,1,1)
plt.ion()
while True:
    print >>sys.stderr, '\nwaiting to receive message'
    data, address = sock.recvfrom(4800)
    
    print >>sys.stderr, 'received %s bytes from %s' % (len(data), address)
    img_tuple = unpack('B'*4800, data)
    print >>sys.stderr, 'unpacked %s bytes from data' % (len(img_tuple))
    img = np.array(img_tuple).reshape((60,80))
    img = np.uint8(img)
    if (squence == 0):
        im1 = ax1.imshow(img)
    else:
        im1.set_data(img)
    plt.pause(0.000001)
    squence += 1
plt.ioff()
plt.show()
    
    # if data:
    #     sent = sock.sendto(data, address)
    #     print >>sys.stderr, 'sent %s bytes back to %s' % (sent, address)