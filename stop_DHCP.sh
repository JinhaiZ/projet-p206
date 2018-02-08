#!/bin/sh

# stop the DHCP server
sudo service isc-dhcp-server stop

# shutdown the eth0 interface
sudo ifdown enp61s0 