#!/bin/sh

# bring up the eth0 interface
sudo ifup enp61s0 

# start DHCP service
sudo service isc-dhcp-server start

# enable forwarding from the ethernet to wireless router
sudo /sbin/iptables -t nat -A POSTROUTING -o wlp62s0 -j MASQUERADE
