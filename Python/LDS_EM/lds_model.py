import utils
import numpy as np
from numpy.linalg import inv as inv
from numpy.linalg import det as det
np.random.seed(1)
epsilon = 1e-6

'''
LDS model class
Input:
    - data: with shape (N, T, P) where N is the number of samples,
        T is the number of time points, and P is the dimension of the data
    - A: is the LxL transition matrix
    - state_Sigma: is the LxL latent covariance matrix
    - mu_0: is the L latent prior mean
    - init_Sigma: is the LxL latent prior variance
    - W: is the PxL loading/observation-system matrix
    - obs_Sigma: is the PxP observation covariance matrix
'''
class LDS_Model():
    def __init__(self, data_dim, latent_dim, num_time, num_samples=1):
        self.P = data_dim
        self.L = latent_dim
        self.T = num_time
        self.N = num_samples
        
        # default observation parameters
        self.W = np.ones((self.P, self.L))
        self.obs_Sigma = np.eye(self.P)
        
        # default states parameters
        self.A = 0.99*utils.random_rotation(self.L, theta=45)
        self.state_Sigma = np.eye(self.L)
        self.mu_0 = np.zeros(self.L)
        self.V_0 = np.eye(self.L)

    '''
    Set observed data - unneccesary if running EM
    '''
    def set_observation_data(self, data):
        self.x = data

    '''
    Run EM algo for given number of iterations
    '''
    def run_EM(self, data, iterations):
        self.x = data
        lls = []
        for iteration in range(iterations):
            self.run_EM_iteration()
            lls.append(self.ll)
            print(str(iteration) + ': '+ str(self.ll))
        return lls

    '''
    Full pass of EM algo
    '''    
    def run_EM_iteration(self):
        self.E_step()
        self._expectations()
        self.M_step()
        self.sample_states()
        self.log_likelihood()

    '''
    E step: estimate the posterior using forward and backward path given parameters
    '''
    def E_step(self):
        # forward path is estimating prediction parameters
        self._forward_filter()
        # backward path estimate the smoothing parameters
        self._backward_smooth()

    '''
    E_step helper: Get filtered posterior: filtered_mu and filtered_V
    '''      
    def _forward_filter(self):
        ### Kalman gain and covariance
        # Initialize
        predicted_V = np.zeros((self.T+1, self.L, self.L))
        Kalman_gain = np.zeros((self.T, self.L, self.P))
        self.filtered_V = np.zeros((self.T, self.L, self.L))
        # Initialize for first time point
        Kalman_gain[0] = np.matmul(np.matmul(self.V_0, self.W.T), \
                            np.linalg.pinv(np.matmul(np.matmul(self.W, self.V_0), self.W.T) + self.obs_Sigma))
        self.filtered_V[0] = np.matmul(np.eye(self.L)-np.matmul(Kalman_gain[0], self.W), self.V_0)
        for t in range(1, self.T):
            # Prediction Step: eq:invariant_filteringVt-1 Bishop 13.88, Murphy 18.28
            predicted_V[t] = (np.matmul(np.matmul(self.A, self.filtered_V[t-1]), self.A.T) + self.state_Sigma)
            ## Measurement step
            # eq:invariant_kalman_gain2 Bishop 13.92 Murphy 18.39
            Kalman_gain[t] = np.matmul(np.matmul(predicted_V[t], self.W.T), 
                                np.linalg.pinv(np.matmul(np.matmul(self.W, predicted_V[t]), \
                                self.W.T) + self.obs_Sigma))
            # eq:invariant_filteringV  Bishop 13.90 Murphy 18.32
            self.filtered_V[t] = np.matmul(np.eye(self.L) - np.matmul(Kalman_gain[t], self.W), predicted_V[t])
        # Address time shift
        predicted_V[-1] = np.matmul(np.matmul(self.A, self.filtered_V[-1]), self.A.T) + self.state_Sigma
        self.predicted_V = predicted_V[1:]       
        ### Means
        # Initialize
        self.filtered_mu = np.zeros((self.N, self.T, self.L))
        predicted_mu = np.zeros((self.N, self.T+1, self.L))
        for n in range(self.N):
            # Initialize means for first time point
            self.filtered_mu[n,0] = self.mu_0 + np.matmul(Kalman_gain[0], self.x[n,0]-np.matmul(self.W, self.mu_0))
            # Forward
            for t in range(1, self.T):
                # Prediction Step: eq:invariant_filteringMut-1 Bishop 13.89 Murphy 18.27
                predicted_mu[n,t] = np.matmul(self.A, self.filtered_mu[n,t-1])
                ## Measurement step
                # eq:invariant_residual Murphy 18.33
                residual = self.x[n,t] - np.matmul(self.W, predicted_mu[n,t])
                # eq:invariant_filteringMu Bishop 13.89 Murphy 18.31
                self.filtered_mu[n,t] = predicted_mu[n,t] + np.matmul(Kalman_gain[t], residual)
            # Adress time shift
            predicted_mu[n,-1] = np.matmul(self.A, self.filtered_mu[n,-1])
        self.predicted_mu = predicted_mu[:,1:]

    '''
    E_step helper: Get smoothed posterior
    '''
    def _backward_smooth(self):
        self.smoothed_mu = np.zeros((self.N, self.T, self.L))
        self.smoothed_V = np.zeros((self.T, self.L, self.L))
        self.backward_Kalman = np.zeros((self.T, self.L, self.L))
        ## Backward Kalman and covariance
        self.smoothed_V[-1] = self.filtered_V[-1]
        self.backward_Kalman[-1] = np.matmul(np.matmul(self.smoothed_V[-1], self.A.T), \
                                    np.linalg.pinv(self.predicted_V[-1]))
        for t in range(self.T-2, -1, -1): #-2 because indexing starts at 0
            self.backward_Kalman[t] = np.matmul(np.matmul(self.filtered_V[t], self.A.T), \
                                    np.linalg.pinv(self.predicted_V[t+1]))
            # eq:invariant_smoothedV Bishop 13.101 Murphy 15.58
            self.smoothed_V[t] = self.filtered_V[t] + np.matmul(self.backward_Kalman[t], np.matmul((self.smoothed_V[t+1] \
                                - self.predicted_V[t+1]), self.backward_Kalman[t].T))
            self.smoothed_V[t] = utils.fix_covariance(self.smoothed_V[t])
        ## Means
        for n in range(self.N):
            self.smoothed_mu[n,-1] = self.filtered_mu[n,-1]
            for t in range(self.T-2, -1, -1): #-2 because indexing starts at 0
                # eq:invariant_smoothedMu Bishop 13.100 Murphy 18.57
                self.smoothed_mu[n,t] = self.filtered_mu[n,t] + np.matmul(self.backward_Kalman[t], 
                                        (self.smoothed_mu[n,t+1] - self.predicted_mu[n,t+1]))

    '''
    Get expections needed for M step
    '''
    def _expectations(self):
        # Initialize
        self.E_z = np.zeros((self.N, self.T, self.L))
        self.E_z_zT = np.zeros((self.N, self.T, self.L, self.L))
        self.E_z_z1T = np.zeros((self.N, self.T-1, self.L, self.L))
        self.E_z1_zT = np.zeros((self.N, self.T-1, self.L, self.L))
        # eq:Expectations Bishop 13.105-13.107
        self.E_z = self.smoothed_mu
        for n in range(self.N):
            self.E_z_zT[n] = [self.smoothed_V[t] + np.matmul(self.smoothed_mu[n,t], 
                            self.smoothed_mu[n,t].T) for t in range(self.T)]
            self.E_z_z1T[n] = [np.matmul(self.backward_Kalman[t-1], self.smoothed_V[t]) +\
                            np.matmul(np.expand_dims(self.smoothed_mu[n,t], 1), 
                            np.expand_dims(self.smoothed_mu[n,t-1],1).T) for t in range(1, self.T)]
            self.E_z1_zT[n] = [np.matmul(self.backward_Kalman[t], self.smoothed_V[t-1]) +\
                            np.matmul(np.expand_dims(self.smoothed_mu[n,t-1], 1),\
                            np.expand_dims(self.smoothed_mu[n,t], 1).T) for t in range(1, self.T)]

    '''
    M step: Update the parameters by maximizing the complete data 
        log likelihood with resprect to the posterior distribution
    '''
    def M_step(self):
       # run M_step tp update the kalman parameters
        self._update_initial_params()
        self._update_state_params()
        self._update_obs_params()

    '''
    M_step helper: update inital mu and v
    '''   
    def _update_initial_params(self):
        # eq:new_mu Bishop 13.110
        self.mu_0 = np.sum(self.smoothed_mu[:,0,:],axis=0)/self.N
        # eq:new_V Bishop 13.111
        self.V_0 = np.sum([self.E_z_zT[n,0] - np.matmul(self.E_z[n,0], self.E_z[n,0].T) \
                    for n in range(self.N)], axis=0)/self.N
        self.V_0 = utils.fix_covariance(self.V_0)

    '''
    M_step helper: update transition matrix and latent Sigma
    '''
    def _update_state_params(self):
        # eq:new_A Bishop 13.113
        self.A = np.sum([np.matmul(np.sum(self.E_z_z1T[n,:,:], axis=0), np.linalg.pinv(np.sum(self.E_z_zT[n,:-1,:], \
                    axis=0))) for n in range(self.N)], axis=0)/self.N
        # eq:new_sigmaz Bishop 13.114
        temp = []
        for n in range(self.N):
            summ = np.zeros((self.L, self.L))
            for t in range(self.T-1):
                summ += self.E_z_zT[n,t,:] - np.matmul(self.A, self.E_z1_zT[n,t,:]) -\
                        np.matmul(self.E_z_z1T[n,t,:], self.A) +\
                        np.matmul(self.A, np.matmul(self.E_z_zT[n,t-1,:], self.A.T))
            temp.append(summ/(self.T-1))
        self.state_Sigma = np.sum(temp, axis=0)/self.N
        self.state_Sigma = utils.fix_covariance(self.state_Sigma)

    '''
    M_step helper: update observation matrix and observation Sigma
    '''
    def _update_obs_params(self):
        # eq:new_W Bishop 13.115
        temp = []
        for n in range(self.N):
            temp.append(np.matmul(sum([np.matmul(np.expand_dims(self.x[n,t], 1), \
                np.expand_dims(self.E_z[n,t], 1).T) for t in range(self.T)]), np.linalg.pinv(sum(self.E_z_zT[n]))))
        self.W = np.sum(temp, axis=0)/self.N
        # eq:new_sigmax Bishop 13.116
        temp = []
        for n in range(self.N):
            x_xT = sum([np.matmul(np.expand_dims(self.x[n,t], 1), np.expand_dims(self.x[n,t], 1).T) \
                    for t in range(self.T)])
            W_E_z_x = sum([np.matmul(self.W, np.matmul(np.expand_dims(self.E_z[n,t], 1), \
                        np.expand_dims(self.x[n,t], 1).T)) for t in range(self.T)])
            x_E_zt_W = sum([np.matmul(np.matmul(np.expand_dims(self.x[n,t], 1), \
                        np.expand_dims(self.E_z[n,t], 1).T), self.W.T) for t in range(self.T)])
            W_E_z_zt_Wt = sum([np.matmul(np.matmul(self.W, self.E_z_zT[n,t]), self.W.T) 
                        for t in range(self.T)])
            temp.append((x_xT - W_E_z_x - x_E_zt_W + W_E_z_zt_Wt)/self.T)
        self.obs_Sigma = np.sum(temp, axis=0)/self.N
        self.obs_Sigma = utils.fix_covariance(self.obs_Sigma)

    '''
    Sample latent states using statistics
    '''
    def sample_states(self):
        self.z = np.zeros((self.N, self.T, self.L))
        for n in range(self.N):
            for t in range(self.T):
                self.z[n,t] = np.random.multivariate_normal(self.smoothed_mu[n,t], self.smoothed_V[t])

    '''
    Calculate complete data log likelihood
    '''
    def log_likelihood(self):
        total_sum = 0
        for n in range(self.N):
            # eq:LLprior
            ll_prior = -self.L/2*np.log(2*np.pi) - 0.5*np.log(det(self.V_0)+epsilon) \
                       - 0.5*np.matmul(np.expand_dims(self.z[n,0]-self.mu_0, 1).T, \
                       np.matmul(np.linalg.pinv(self.V_0), np.expand_dims(self.z[n,0]-self.mu_0, 1)))
            # eq:LLstate
            ll_state = -((self.T-1)*self.L)/2*np.log(2*np.pi) - ((self.T-1)/2)*np.log(det(self.state_Sigma)+epsilon) \
                    - 0.5*sum([np.matmul(np.expand_dims(self.z[n,t] - np.matmul(self.A, self.z[n,t-1]), 1).T, \
                    np.matmul(np.linalg.pinv(self.state_Sigma), np.expand_dims(self.z[n,t] \
                    - np.matmul(self.A, self.z[n,t-1]), 1))) for t in range(1, self.T)])
           # eq:LLobs         
            ll_obs = -(self.T*self.P)/2*np.log(2*np.pi) - (self.T/2)*np.log(det(self.obs_Sigma)+epsilon) \
                    - 0.5*sum([np.matmul(np.expand_dims(self.x[n,t] - np.matmul(self.W, self.z[n,t]), 1).T, \
                    - np.matmul(np.linalg.pinv(self.obs_Sigma),np.expand_dims(self.x[n,t] \
                    - np.matmul(self.W,self.z[n,t]), 1))) for t in range(self.T)])
            # eq:completeLL
            total_sum += ll_prior + ll_state + ll_obs
        # eq:EM
        total_sum = np.squeeze(total_sum)
        self.ll = total_sum/self.N
        return total_sum/self.N

    '''
    TODO: check this
    Get log prob of data
    '''
    def data_log_prob(self, data):
        self.x = data
        self._forward_filter()
        log_probs = []
        for n in range(self.N):
            n_log_prob = -(self.T*self.P)/2*np.log(2*np.pi) - 0.5*sum([ \
                        np.log(det(np.matmul(self.W, np.matmul(self.predicted_V[t], self.W.T)) + self.obs_Sigma)) \
                        + np.matmul(np.expand_dims(self.x[n,t] - np.matmul(self.W, self.predicted_mu[n,t]), 1).T, \
                        np.matmul(np.linalg.pinv(np.matmul(self.W, np.matmul(self.predicted_V[t], self.W.T)) + self.obs_Sigma),\
                        np.expand_dims(self.x[n,t] - np.matmul(self.W,self.predicted_mu[n,t]), 1))) \
                        for t in range(self.T)])
            log_probs.append(n_log_prob[0][0])
        return np.array(log_probs)

    '''
    Sample observations and latent from model
    '''
    def sample(self, num_samples):
        z = np.zeros((num_samples, self.T+1, self.L))
        x = np.zeros((num_samples, self.T+1, self.P))
        for n in range(num_samples):
            z[n,0] = np.random.multivariate_normal(self.mu_0, self.V_0)
            for t in range(1, self.T+1):
                z[n,t] = np.random.multivariate_normal(np.matmul(self.A, z[n,t-1]), self.state_Sigma)
                x[n,t] = np.random.multivariate_normal(np.matmul(self.W, z[n,t]), self.obs_Sigma)
        return x[:,1:,:], z[:,1:,:]
