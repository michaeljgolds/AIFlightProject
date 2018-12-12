"""
This part of code is the reinforcement learning brain, which is a brain of the agent.
All decisions are made in here.

Policy Gradient, Reinforcement Learning.

View more on my tutorial page: https://morvanzhou.github.io/tutorials/

Using:
Tensorflow: 1.0
gym: 0.8.0
"""

import numpy as np
import tensorflow as tf

# reproducible
np.random.seed(1)
tf.set_random_seed(1)


class PolicyGradient:
    def __init__(
            self,
            n_actions,
            n_features,
            learning_rate=0.01,
            reward_decay=0.95,
            output_graph=False,
    ):
        self.n_actions = n_actions
        self.n_features = n_features
        self.lr = learning_rate
        self.gamma = reward_decay

        self.ep_obs, self.ep_as, self.ep_rs = [], [], []

        self._build_net()

        self.sess = tf.Session()

        if output_graph:
            # $ tensorboard --logdir=logs
            # http://0.0.0.0:6006/
            # tf.train.SummaryWriter soon be deprecated, use following
            tf.summary.FileWriter("logs/", self.sess.graph)


        

        self.saver = tf.train.Saver()
        try:
            self.saver.restore(self.sess, "./model.ckpt")
        except:
            print("Didn't load model, no file")
            self.sess.run(tf.global_variables_initializer())
        
        self.loss_out_file = open("loss_out.txt","a+")
        

    def _build_net(self):
        with tf.name_scope('inputs'):
            self.tf_obs = tf.placeholder(tf.float32, [None, self.n_features], name="observations")
            self.tf_act_ele = tf.placeholder(tf.int32, [None, ], name="actions_ele")
            self.tf_act_rud = tf.placeholder(tf.int32, [None, ], name="actions_rud")
            self.tf_act_roll = tf.placeholder(tf.int32, [None, ], name="actions_roll")
            self.tf_act_shoot = tf.placeholder(tf.int32, [None, ], name="actions_shoot")
            self.tf_act_thrust = tf.placeholder(tf.int32, [None, ], name="actions_thrust")
            self.tf_vt = tf.placeholder(tf.float32, [None, ], name="actions_value")
        # fc1
        layer1 = tf.layers.dense(
            inputs=self.tf_obs,
            units=10,
            activation=tf.nn.relu6,  
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='fc1',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        )
        layer2 = tf.layers.dense(
            inputs=layer1,
            units=10,
            activation=tf.nn.relu6, 
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='fc2',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        )
        layer3 = tf.layers.dense(
            inputs=layer2,
            units=10,
            activation=tf.nn.relu6, 
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='fc3',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        )
        layer4 = tf.layers.dense(
            inputs=layer3,
            units=5,
            activation=tf.nn.relu6, 
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='fc4',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        )
        layer = tf.layers.dense(
            inputs=self.tf_obs,
            units=10,
            activation=tf.nn.tanh, 
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='fc5',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        )
        #layer1_print = tf.Print(layer,[layer],"layer 1: ")
        
        #layer2 = tf.layers.dense(
        #   inputs=layer1_print,
        #    units=256,
        #    activation=tf.nn.relu,  # relu activation
        #    kernel_initializer=tf.contrib.layers.xavier_initializer(),
        #    name='fc2',
        #    use_bias=False
        #)
        #layer2_print = tf.Print(layer2,[layer2],"layer 2: ")
        #layer3 = tf.layers.dense(
        #    inputs=layer2_print,
        #    units=256,
        #    activation=tf.nn.relu,  # tanh activation
        #    kernel_initializer=tf.contrib.layers.xavier_initializer(),
        #    name='fc3',
        #    use_bias=False
        #)
        #layer3_print = tf.Print(layer3,[layer3],"layer 3: ")
         #fc2
        act_ele = tf.layers.dense(
            inputs=layer,
            units=3,
            activation=None,
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='act_ele',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        ) 
        
        act_rud = tf.layers.dense(
            inputs=layer,
            units=3,
            activation=None,
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='act_rud',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        ) 
        
        act_roll = tf.layers.dense(
            inputs=layer,
            units=3,
            activation=None,
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='act_roll',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        ) 
        act_shoot = tf.layers.dense(
            inputs=layer,
            units=2,
            activation=None,
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='act_shoot',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        )
        act_thrust = tf.layers.dense(
            inputs=layer,
            units=2,
            activation=None,
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='act_thrust',
            use_bias=True,
            bias_initializer=tf.constant_initializer(0.01)
        )
        self.act_prob_ele = tf.nn.softmax(act_ele, name='act_prob_ele')
        self.act_prob_rud = tf.nn.softmax(act_rud, name='act_prob_rud')
        self.act_prob_roll = tf.nn.softmax(act_roll, name='act_prob_roll')
        self.act_prob_shoot = tf.nn.softmax(act_shoot, name='act_prob_shoot')
        self.act_prob_thrust = tf.nn.softmax(act_thrust, name='act_prob_thrust')
        
        #all_act = tf.layers.dense(
          #  inputs=layer,
           # units=self.n_actions,
          #  activation=None,
         #   kernel_initializer=tf.random_normal_initializer(mean=0, stddev=0.3),
        #    name='fc4',
        #    use_bias=True,
        #    bias_initializer=tf.constant_initializer(0.1)
        #)
        #all_act_print = tf.Print(all_act,[all_act],"layer all_act: ")
        
        #self.all_act_prob = tf.nn.softmax(all_act, name='act_prob')  # use softmax to convert to probability

        #with tf.name_scope('loss'):
            # to maximize total reward (log_p * R) is to minimize -(log_p * R), and the tf only have minimize(loss)
            #neg_log_prob = tf.nn.sparse_softmax_cross_entropy_with_logits(logits=all_act, labels=self.tf_acts)   # this is negative log of chosen action
            # or in this way:
            # neg_log_prob = tf.reduce_sum(-tf.log(self.all_act_prob)*tf.one_hot(self.tf_acts, self.n_actions), axis=1)
            #self.loss = tf.reduce_mean(neg_log_prob * self.tf_vt)  # reward guided loss
            #self.loss_print = tf.Print(self.loss,[self.loss],"loss: ")
        neg_log_prob_ele = tf.nn.sparse_softmax_cross_entropy_with_logits(logits=act_ele, labels=self.tf_act_ele)
        neg_log_prob_rud = tf.nn.sparse_softmax_cross_entropy_with_logits(logits=act_rud, labels=self.tf_act_rud)
        neg_log_prob_roll = tf.nn.sparse_softmax_cross_entropy_with_logits(logits=act_roll, labels=self.tf_act_roll)
        neg_log_prob_shoot = tf.nn.sparse_softmax_cross_entropy_with_logits(logits=act_shoot, labels=self.tf_act_shoot)
        neg_log_prob_thrust = tf.nn.sparse_softmax_cross_entropy_with_logits(logits=act_thrust, labels=self.tf_act_thrust)
        self.loss_ele = tf.reduce_mean(neg_log_prob_ele * self.tf_vt)
        self.loss_rud = tf.reduce_mean(neg_log_prob_rud * self.tf_vt)
        self.loss_roll = tf.reduce_mean(neg_log_prob_roll * self.tf_vt)
        self.loss_shoot = tf.reduce_mean(neg_log_prob_shoot * self.tf_vt)
        self.loss_thrust = tf.reduce_mean(neg_log_prob_thrust * self.tf_vt)
        self.loss_all = tf.reduce_mean((neg_log_prob_ele + neg_log_prob_rud + neg_log_prob_roll + neg_log_prob_thrust) * self.tf_vt)
        #with tf.name_scope('train'):
        self.train_op_ele = tf.train.AdamOptimizer(self.lr).minimize(self.loss_ele)
        self.train_op_rud = tf.train.AdamOptimizer(self.lr).minimize(self.loss_rud)
        self.train_op_roll = tf.train.AdamOptimizer(self.lr).minimize(self.loss_roll)
        self.train_op_shoot = tf.train.AdamOptimizer(self.lr).minimize(self.loss_shoot)
        self.train_op_thrust = tf.train.AdamOptimizer(self.lr).minimize(self.loss_thrust)
        self.train_op_all = tf.train.AdamOptimizer(self.lr).minimize(self.loss_all)
        
        
    def choose_action(self, observation):
        prob_weight_ele, prob_weight_rud, prob_weight_roll, prob_weight_shoot, prob_weight_thrust = self.sess.run([self.act_prob_ele, self.act_prob_rud, self.act_prob_roll, self.act_prob_shoot, self.act_prob_thrust], feed_dict={self.tf_obs: observation[np.newaxis, :]})
        action = [0,0,0,0,0]
        ele_ravel = prob_weight_ele.ravel().tolist()
        ele_ravel[ele_ravel.index(max(ele_ravel))] -= 0.03
        ele_ravel = [val + 0.01 for val in ele_ravel]
        ele_ravel = [val/sum(ele_ravel) for val in ele_ravel]
        rud_ravel = prob_weight_rud.ravel().tolist()
        rud_ravel[rud_ravel.index(max(rud_ravel))] -= 0.03
        rud_ravel = [val + 0.01 for val in rud_ravel]
        rud_ravel = [val/sum(rud_ravel) for val in rud_ravel]
        roll_ravel = prob_weight_roll.ravel().tolist()
        roll_ravel[roll_ravel.index(max(roll_ravel))] -= 0.03
        roll_ravel = [val + 0.01 for val in roll_ravel]
        roll_ravel = [val/sum(roll_ravel) for val in roll_ravel]
        shoot_ravel = prob_weight_shoot.ravel().tolist()
        shoot_ravel[shoot_ravel.index(max(shoot_ravel))] -= 0.02
        shoot_ravel = [val + 0.01 for val in shoot_ravel]
        shoot_ravel = [val/sum(shoot_ravel) for val in shoot_ravel]
        thrust_ravel = prob_weight_thrust.ravel().tolist()
        thrust_ravel[thrust_ravel.index(max(thrust_ravel))] -= 0.02
        thrust_ravel = [val + 0.01 for val in thrust_ravel]
        thrust_ravel = [val/sum(thrust_ravel) for val in thrust_ravel]
        print([prob_weight_ele.ravel(), prob_weight_rud.ravel(), prob_weight_roll.ravel(), prob_weight_shoot.ravel(), prob_weight_thrust.ravel()])
        print([ele_ravel, rud_ravel, roll_ravel, shoot_ravel, thrust_ravel])
        print([sum(ele_ravel), sum(rud_ravel), sum(roll_ravel), sum(shoot_ravel), sum(thrust_ravel)])
        #action[0] = np.random.choice(range(prob_weight_ele.shape[1]), p=prob_weight_ele.ravel()) # select action w.r.t the actions prob
        #action[1] = np.random.choice(range(prob_weight_rud.shape[1]), p=prob_weight_rud.ravel())
        #action[2] = np.random.choice(range(prob_weight_roll.shape[1]), p=prob_weight_roll.ravel())
        #action[3] = np.random.choice(range(prob_weight_shoot.shape[1]), p=prob_weight_shoot.ravel())
        #action[4] = np.random.choice(range(prob_weight_thrust.shape[1]), p=prob_weight_thrust.ravel())
        action[0] = np.random.choice(range(prob_weight_ele.shape[1]), p=ele_ravel) # select action w.r.t the actions prob
        action[1] = np.random.choice(range(prob_weight_rud.shape[1]), p=rud_ravel)
        action[2] = np.random.choice(range(prob_weight_roll.shape[1]), p=roll_ravel)
        action[3] = np.random.choice(range(prob_weight_shoot.shape[1]), p=shoot_ravel)
        action[4] = np.random.choice(range(prob_weight_thrust.shape[1]), p=thrust_ravel)
        #print(prob_weights[0][action])
        #print(action)
        return action

    def store_transition(self, s, a, r):
        self.ep_obs.append(s)
        self.ep_as.append(a)
        self.ep_rs.append(r)

    def learn(self):
        # discount and normalize episode reward
        discounted_ep_rs_norm = self._discount_and_norm_rewards()

        #print(discounted_ep_rs_norm)
        #print(tf.trainable_variables())
        gr = tf.get_default_graph()
        #print("layer 1 tensor before training")
        #print(gr.get_tensor_by_name('fc1/kernel:0').eval(session=self.sess))
        #layer1before = gr.get_tensor_by_name('fc1/kernel:0').eval(session=self.sess)
        #layer2before = gr.get_tensor_by_name('fc4/kernel:0').eval(session=self.sess)
        
        #self.sess.run(self.loss_print, feed_dict={
        #     self.tf_obs: np.vstack(self.ep_obs),  # shape=[None, n_obs]
        #    self.tf_acts: np.array(self.ep_as),  # shape=[None, ]
        #     self.tf_vt: discounted_ep_rs_norm,  # shape=[None, ]
        #})
        
        # train on episode
        
        #self.sess.run([self.train_op_ele, self.train_op_rud, self.train_op_roll, self.train_op_shoot, self.train_op_thrust], feed_dict={
        _, loss_val = self.sess.run([self.train_op_all, self.loss_all], feed_dict={
             self.tf_obs: np.vstack(self.ep_obs),  # shape=[None, n_obs]
             self.tf_act_ele: np.array([act[0] for act in self.ep_as]),  # shape=[None, ]
             self.tf_act_rud: np.array([act[1] for act in self.ep_as]),  # shape=[None, ]
             self.tf_act_roll: np.array([act[2] for act in self.ep_as]),  # shape=[None, ]
             self.tf_act_shoot: np.array([act[3] for act in self.ep_as]),  # shape=[None, ]
             self.tf_act_thrust: np.array([act[4] for act in self.ep_as]),  # shape=[None, ]
             self.tf_vt: discounted_ep_rs_norm,  # shape=[None, ]
        })

        print(loss_val)
        
        self.loss_out_file = open("loss_out.txt","a+")
        self.loss_out_file.write(str(loss_val)+"\n")
        self.loss_out_file.close()
        
        #layer1after = gr.get_tensor_by_name('fc1/kernel:0').eval(session=self.sess)
        #layer2after = gr.get_tensor_by_name('fc4/kernel:0').eval(session=self.sess)
        #print("layer 1 tensor after training")
        #print(gr.get_tensor_by_name('fc1/kernel:0').eval(session=self.sess))
        
        #print("Non-zero elements in difference layer 1:")
        #np.set_printoptions(threshold=np.nan)
        #print(np.transpose(np.nonzero(layer1after-layer1before)))
        
        
        #print("Non-zero elements in difference layer 2:")
        #np.set_printoptions(threshold=np.nan)
        #print(np.transpose(np.nonzero(layer2after-layer2before)))
        
        self.ep_obs, self.ep_as, self.ep_rs = [], [], []    # empty episode data
        return discounted_ep_rs_norm

    def _discount_and_norm_rewards(self):
        
        #return discounted_ep_rs
        # normalize episode rewards
        if len(self.ep_rs) > 1000:
            
            discounted_ep_rs = np.zeros_like(self.ep_rs)
            running_add = 0
            for t in reversed(range(0, len(self.ep_rs))):
                running_add = running_add * self.gamma + self.ep_rs[t]
                discounted_ep_rs[t] = running_add
        
            discounted_ep_rs -= np.mean(discounted_ep_rs)
            discounted_ep_rs /= np.std(discounted_ep_rs)
            return discounted_ep_rs
        else:
            
            discounted_ep_rs = np.zeros_like(self.ep_rs)
            running_add = 0
            for t in reversed(range(0, 1000)):
                if t >= len(self.ep_rs):
                    running_add = running_add * self.gamma
                else:
                    running_add = running_add * self.gamma + self.ep_rs[t]
                    discounted_ep_rs[t] = running_add
        
            discounted_ep_rs -= np.mean(np.concatenate((discounted_ep_rs, np.zeros_like([0]*(1000-len(self.ep_rs))))))
            discounted_ep_rs /= np.std(np.concatenate((discounted_ep_rs, np.zeros_like([0]*(1000-len(self.ep_rs))))))
            return discounted_ep_rs


    def SaveNet(self):
        save_path = self.saver.save(self.sess, "./model.ckpt")
        #print ("saved to %s" % save_path)
        
