

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

RL = PolicyGradient(
    n_actions=5,
    n_features=20,
    learning_rate=0.02,
    reward_decay=0.99,
    # output_graph=True,
)


context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5555")

waitasec = 0

for i_episode in range(3):
	message = socket.recv()
	print("Received request: %s" % message)
	state = [float(f) for f in message.decode().split(' ')]
	print("Player x,y,z: " + str(state[0:3]))
	print("Player speed: " + str(state[3]))

	observation = np.array(state[0:20])    

	while True:
	    #  Wait for next request from client
		action = RL.choose_action(observation)

		print(action)

		print(str(action[0]) + " " + str(action[1]) + " " + str(action[2]) + " " + str(action[3]) + " " + str(action[4]) + "          ")
		socket.send_string(str(action[0]) + " " + str(action[1]) + " " + str(action[2]) + " " + str(action[3]) + " " + str(action[4]) + "          ")
		message = socket.recv()
		print("Received request: %s" % message)
		state = [float(f) for f in message.decode().split(' ')]
		print("Player x,y,z: " + str(state[0:3]))
		print("Player speed: " + str(state[3]))

		observation_ = np.array(state[0:20])
		reward = state[20]
		done = False
		
		RL.store_transition(observation, action, reward)
		
		if reward != 0:
			done = True

		if done:
			ep_rs_sum = sum(RL.ep_rs)

			if 'running_reward' not in globals():
				running_reward = ep_rs_sum
			else:
				running_reward = running_reward * 0.99 + ep_rs_sum * 0.01
			print("episode:", i_episode, "  reward:", int(running_reward))

			vt = RL.learn()

			if i_episode == 0:
				plt.plot(vt)    # plot the episode vt
				plt.xlabel('episode steps')
				plt.ylabel('normalized state-action value')
				plt.show()
			break

		observation = observation_
		#  Do some 'work'
		time.sleep(0.038)
