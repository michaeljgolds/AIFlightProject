

#
#   Hello World server in Python
#   Binds REP socket to tcp://*:5555
#   Expects b"Hello" from client, replies with b"World"
#

import time
import zmq
import random

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5555")

waitasec = 0
action = [random.randint(-1,1),random.randint(-1,1), random.randint(-1,1), random.randint(0,1), random.randint(-1,1)]
        
while True:
    #  Wait for next request from client
    message = socket.recv()
    print("Received request: %s" % message)

    #  Do some 'work'
    time.sleep(0.038)

    waitasec = waitasec + 0.38

    if waitasec > 1:
        action = [random.randint(-1,1),random.randint(-1,1), random.randint(-1,1), random.randint(0,1), random.randint(-1,1)]
        waitasec = 0
    

    #  Send reply back to client
    socket.send_string(str(action[0]) + " " + str(action[1]) + " " + str(action[2]) + " " + str(action[3]) + " " + str(action[4]) + "          ")
