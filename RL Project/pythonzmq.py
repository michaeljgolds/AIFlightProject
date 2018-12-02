

#
#   Hello World server in Python
#   Binds REP socket to tcp://*:5555
#   Expects b"Hello" from client, replies with b"World"
#

import time
import zmq
import random
from RL_brain import PolicyGradient
import matplotlib.pyplot as plt
import numpy as np

def normalizeState(state):
    state[0] = state[0] / 100           #player x
    state[1] = (state[1] - 25)/38       #player y
    state[2] = state[2] / 100          #player z
    state[3] = (state[3] - 0.333 ) * 3  #player speed
    #state[4] = (state[4] - b ) * m  #accx
    #state[5] = (state[5] - b ) * m  #accy
    #state[6] = (state[6] - b ) * m  #accz
    state[7] = (state[7] - 180 ) / 180  #player phi
    state[8] = (state[8] - 180 ) / 90 #player gamma
    state[9] = (state[9] - 0 ) / 180 #player theta
    #state[10] = (state[10] - b ) * m  #elevator effect
    #state[11] = (state[11] - b ) * m  #rudder effect
    #state[12] = (state[12] - b ) * m  #roll effect
    state[13] = (state[13] - 0 ) / 100  #enemy x
    state[14] = (state[14]-25) / 38   #enemy y
    state[15] = (state[15] - 0 ) / 100  #enemy z
    state[16] = (state[16] - 0.333 ) * 3  #enemy speed
    state[17] = (state[17] - 180 ) / 180  #enemy phi
    state[18] = (state[18] - 180 ) / 180 #enemy gamma
    state[19] = (state[19] - 0 ) / 180 #enemy theta
    
RL = PolicyGradient(
    n_actions=108,
    n_features=20,
    learning_rate=0.001,
    reward_decay=0.99,
    output_graph=True,
)

actionList = ["11111","11110","11101","11100","11011","11010","11001","11000","11211","11210","11201","11200","10111","10110","10101","10100","10011","10010","10001","10000","10211","10210","10201","10200","12111","12110","12101","12100","12011","12010","12001","12000","12211","12210","12201","12200","01111","01110","01101","01100","01011","01010","01001","01000","01211","01210","01201","01200","00111","00110","00101","00100","00011","00010","00001","00000","00211","00210","00201","00200","02111","02110","02101","02100","02011","02010","02001","02000","02211","02210","02201","02200","21111","21110","21101","21100","21011","21010","21001","21000","21211","21210","21201","21200","20111","20110","20101","20100","20011","20010","20001","20000","20211","20210","20201","20200","22111","22110","22101","22100","22011","22010","22001","22000","22211","22210","22201","22200"]

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5555")

waitasec = 0

for i_episode in range(30000):
    message = socket.recv()
    #print("Received request: %s" % message)
    state = [float(f) for f in message.decode().split(' ')]
    normalizeState(state)
    #print("Player x,y,z: " + str(state[0:3]))
    #print("Player phi, gamma, theta: " + str(state[7:10]))
    #print("Enemy x,y,z: " + str(state[13:16]))
    #print("Enemy phi, gamma, theta: " + str(state[17:20]))
    #print(i_episode)
    observation = np.array(state[0:20])
    while True:
    #  Wait for next request from client
        action = RL.choose_action(observation)
        #action = actionList[actionNumber]
        # print(action)

    # print(str(action[0]) + " " + str(action[1]) + " " + str(action[2]) + " " + str(action[3]) + " " + str(action[4]) + "          ")
        socket.send_string(str(action[0]) + " " + str(action[1]) + " " + str(action[2]) + " 0 " + str(action[4]) + "          ")
        message = socket.recv()
        #print("Received request: %s" % message)
        state = [float(f) for f in message.decode().split(' ')]
        #print(state[13:16])
        normalizeState(state)
        #print("Player x,y,z: " + str(state[0:3]))
        #print("Player speed: " + str(state[3]))
        #print("Player accx,accy,accz: " + str(state[4:7]))
        #print("Player phi,gamma,theta: " + str(state[7:10]))
        #print("Player elevator,rudder,roller: " + str(state[10:13]))
        #print("Enemy x,y,z: " + str(state[13:16]))
        #print("Enemy speed: " + str(state[16]))
        #print("Enemy phi, gamma, theta: " + str(state[17:20]))

        
        observation_ = np.array(state[0:20])
        reward = state[20]
        donefloat = state[21]
        done = False

        RL.store_transition(observation, action, reward)
        
        if donefloat == 1 or len(RL.ep_rs) > 1000:
            done = True

        if done:
            ep_rs_sum = sum(RL.ep_rs)
            
            r_sum_file = open("reward_out.txt","a+")
            if len(RL.ep_rs) > 1000:
                r_sum_file.write(str(sum(RL.ep_rs[0:1000]))+"\n")
            else:
                r_sum_file.write(str(sum(RL.ep_rs))+"\n")
            r_sum_file.close()
            
            if 'running_reward' not in globals():
                running_reward = ep_rs_sum
            else:
                running_reward = running_reward * 0.99 + ep_rs_sum * 0.01
            #print("episode:", i_episode, "  reward:", running_reward)

            vt = RL.learn()
            
            RL.SaveNet()
            
            socket.send_string(str(action[0]) + " " + str(action[1]) + " " + str(action[2]) + " " + str(action[3]) + " " + str(action[4]) + "          ")
            break            

        observation = observation_
        
        
        #  Do some 'work'
        time.sleep(0.01)
