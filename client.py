#!/usr/bin/python
# -*- coding: UTF-8 -*-

import socket

s = socket.socket()         
host = socket.gethostname()
port = 8888

s.connect((host, port))
print s.recv(1024)
s.send("I'am python simple client. hello server");
s.close()
