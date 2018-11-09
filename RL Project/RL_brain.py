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
        
        
        

    def _build_net(self):
        with tf.name_scope('inputs'):
            self.tf_obs = tf.placeholder(tf.float32, [None, self.n_features], name="observations")
            self.tf_acts = tf.placeholder(tf.int32, [None, ], name="actions_num")
            self.tf_vt = tf.placeholder(tf.float32, [None, ], name="actions_value")
        # fc1
        layer = tf.layers.dense(
            inputs=self.tf_obs,
            units=1024,
            activation=tf.nn.tanh,  # relu activation
            kernel_initializer=tf.contrib.layers.xavier_initializer(),
            name='fc1',
            use_bias=False
        )
        layer1_print = tf.Print(layer,[layer],"layer 1: ")
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
        all_act = tf.layers.dense(
            inputs=layer1_print,
            units=self.n_actions,
            activation=None,
            kernel_initializer=tf.random_normal_initializer(mean=0, stddev=0.3),
            name='fc4',
            use_bias=False
        )
        all_act_print = tf.Print(all_act,[all_act],"layer all_act: ")
        
        self.all_act_prob = tf.nn.softmax(all_act_print, name='act_prob')  # use softmax to convert to probability

        with tf.name_scope('loss'):
            # to maximize total reward (log_p * R) is to minimize -(log_p * R), and the tf only have minimize(loss)
            neg_log_prob = tf.nn.sparse_softmax_cross_entropy_with_logits(logits=all_act, labels=self.tf_acts)   # this is negative log of chosen action
            # or in this way:
            # neg_log_prob = tf.reduce_sum(-tf.log(self.all_act_prob)*tf.one_hot(self.tf_acts, self.n_actions), axis=1)
            loss = tf.reduce_mean(neg_log_prob * self.tf_vt)  # reward guided loss
            self.loss_print = tf.Print(loss,[loss],"loss: ")
            
        with tf.name_scope('train'):
            self.train_op = tf.train.AdamOptimizer(self.lr).minimize(loss)
        
        
    def choose_action(self, observation):
        prob_weights = self.sess.run(self.all_act_prob, feed_dict={self.tf_obs: observation[np.newaxis, :]})
        action = np.random.choice(range(prob_weights.shape[1]), p=prob_weights.ravel())  # select action w.r.t the actions prob
        print(prob_weights[0][action])
        print(action)
        return action

    def store_transition(self, s, a, r):
        self.ep_obs.append(s)
        self.ep_as.append(a)
        self.ep_rs.append(r)

    def learn(self):
        # discount and normalize episode reward
        discounted_ep_rs_norm = self._discount_and_norm_rewards()

        #print(discounted_ep_rs_norm)
        print(tf.trainable_variables())
        gr = tf.get_default_graph()
        print("layer 1 tensor before training")
        #print(gr.get_tensor_by_name('fc1/kernel:0').eval(session=self.sess))
        layer1before = gr.get_tensor_by_name('fc1/kernel:0').eval(session=self.sess)
        layer2before = gr.get_tensor_by_name('fc4/kernel:0').eval(session=self.sess)
        
        self.sess.run(self.loss_print, feed_dict={
             self.tf_obs: np.vstack(self.ep_obs),  # shape=[None, n_obs]
             self.tf_acts: np.array(self.ep_as),  # shape=[None, ]
             self.tf_vt: discounted_ep_rs_norm,  # shape=[None, ]
        })
        
        # train on episode
        self.sess.run(self.train_op, feed_dict={
             self.tf_obs: np.vstack(self.ep_obs),  # shape=[None, n_obs]
             self.tf_acts: np.array(self.ep_as),  # shape=[None, ]
             self.tf_vt: discounted_ep_rs_norm,  # shape=[None, ]
        })

        
        layer1after = gr.get_tensor_by_name('fc1/kernel:0').eval(session=self.sess)
        layer2after = gr.get_tensor_by_name('fc4/kernel:0').eval(session=self.sess)
        print("layer 1 tensor after training")
        #print(gr.get_tensor_by_name('fc1/kernel:0').eval(session=self.sess))
        
        print("Non-zero elements in difference layer 1:")
        #np.set_printoptions(threshold=np.nan)
        print(np.transpose(np.nonzero(layer1after-layer1before)))
        
        
        print("Non-zero elements in difference layer 2:")
        #np.set_printoptions(threshold=np.nan)
        print(np.transpose(np.nonzero(layer2after-layer2before)))
        
        self.ep_obs, self.ep_as, self.ep_rs = [], [], []    # empty episode data
        return discounted_ep_rs_norm

    def _discount_and_norm_rewards(self):
        
        #return discounted_ep_rs
        # normalize episode rewards
        if len(self.ep_rs) > 1000:
            
            discounted_ep_rs = np.zeros_like(self.ep_rs)
            running_add = 0
            for t in reversed(range(0, len(self.ep_rs))):
                running_add = running_add * self.gamma + 1000 * self.ep_rs[t]
                discounted_ep_rs[t] = running_add
        
            #discounted_ep_rs -= np.mean(discounted_ep_rs)
            #discounted_ep_rs /= np.std(discounted_ep_rs)
            return discounted_ep_rs
        else:
            
            discounted_ep_rs = np.zeros_like(self.ep_rs)
            running_add = 0
            for t in reversed(range(0, 1000)):
                if t >= len(self.ep_rs):
                    running_add = running_add * self.gamma - 1000
                else:
                    running_add = running_add * self.gamma + self.ep_rs[t]
                    discounted_ep_rs[t] = running_add
        
            #discounted_ep_rs -= np.mean(np.concatenate((discounted_ep_rs, np.zeros_like([-1000]*(1000-len(self.ep_rs))))))
            #discounted_ep_rs /= np.std(np.concatenate((discounted_ep_rs, np.zeros_like([-1000]*(1000-len(self.ep_rs))))))
            return discounted_ep_rs


    def SaveNet(self):
        save_path = self.saver.save(self.sess, "./model.ckpt")
        print ("saved to %s" % save_path)
        
