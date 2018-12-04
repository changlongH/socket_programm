
import sys
import socket
import time
from gevent.socket import socket as gsocket
from gevent import monkey, spawn, joinall
monkey.patch_all(thread=False)

host = "127.0.0.1"
port = 8888

class client(object):
    def __init__(self, name):
        self.name = name
        self.socket = gsocket(socket.AF_INET,socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.socket.connect((host, port));

    def RecvMsg(self):
        msg = self.socket.recv(1024)
        #print msg

    def SendMsg(self):
        self.socket.send("hello i am No" + self.name);

    def DisConnected(self):
        self.socket.close()
        #self.socket.shutdown(socket.SHUT_RDWR)

def Send(self):
    while(1):
        time.sleep(1)
        self.SendMsg()
        start = time.time()
        self.RecvMsg()
        end = time.time()
        delay =  end - start

        #if delay > 0.01 :
        #    print delay


def TestDelay(clientNum):
    geventList = list()
    for i in range(clientNum):
        c = client(str(i))
        #c.RecvMsg();

        s = spawn(Send, c)
        geventList.append(s)
    joinall(geventList)


def NewClient(id):
    flg = True
    #while flg:
    c = client(str(id))
    c.SendMsg()
    c.RecvMsg()
    c.DisConnected()
    #time.sleep(5) #time_wait 2mls

def TestReconnect(clientNum):
    geventList = list()
    for i in range(clientNum):
        s = spawn(NewClient, i)
        geventList.append(s)
    joinall(geventList)
    

if __name__ == "__main__":
    TestDelay(1000);
    #TestReconnect(300)



