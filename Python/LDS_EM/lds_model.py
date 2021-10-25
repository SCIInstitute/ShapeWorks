import utils
import numpy as np
from numpy.linalg import inv as inv
from numpy.linalg import det as det

'''
Observation parameters class
    Input: P - dimension of observation space, L - dimension of latent space
    - W: is the PxL loading/observation-system matrix
    - obs_Sigma: is the PxP observation covariance matrix
'''
class Obs_params():
    def __init__(self, P, L):
        # Defaults
        self.W = np.ones((P, L))
        self.obs_Sigma = 0.1*np.eye(P)
    def set_W(W):
        self.W = W
    def set_obs_Sigma(obs_Sigma):
        self.obs_sigma = obs_Sigma

'''
State parameters class
    Input: L - dimension of latent space
    - trans_matrix: is the LxL transition matrix
    - state_Sigma: is the LxL latent covariance matrix
    - init_mu: is the L latent prior mean
    - init_Sigma: is the LxL latent prior variance
'''
class State_params():
    def __init__(self,L):
        # defaults
        self.A = 0.99*utils.random_rotation(L)
        self.state_Sigma = 0.1*np.eye(L)
        self.init_mu = np.ones(L)
        self.init_V = np.eye(L)
    def set_A(A):
        self.A = A
    def set_state_Sigma(state_Sigma):
        self.state_Sigma = state_Sigma
    def set_init_mu(init_mu):
        self.init_mu = init_mu
    def set_init_V(init_V):
        self.init_V = init_V

'''
LDS model class
TODO: update to handle data with shape (N,T,P)
Input:
    - data: with shape (N, T, P) where N is the number of samples,
        T is the number of time points, and P is the dimension of the data
    - obs_params: class of observation params
    - state_params: class of state params
'''
class LDS_Model():
    def __init__(self, data, obs_params=None, state_params=None):
        # observation data
        self.X = data
        self.N, self.T, self.P = data.shape
        
        # observation parameters
        self.W = obs_params.W
        self.obs_Sigma = obs_params.obs_Sigma
        
        # states parameters
        self.mu_0 = state_params.init_mu
        self.V_0 = state_params.init_V
        self.A = state_params.A
        self.state_Sigma = state_params.state_Sigma

        # dimensions
        self.L = self.A.shape[0]        

        # E step initialize
        self.predicted_mu = np.zeros((self.N, self.T, self.L))
        self.predicted_V = np.zeros((self.N, self.T, self.L, self.L))
        self.filtered_mu = np.zeros((self.N, self.T, self.L))
        self.filtered_V = np.zeros((self.N, self.T, self.L, self.L))
        self.smoothed_mu = np.zeros((self.N, self.T, self.L))
        self.smoothed_V = np.zeros((self.N, self.T, self.L, self.L))
        self.backward_Kalman = np.zeros((self.N, self.T, self.L, self.L))
        # Expectations initialize
        self.E_z = np.zeros((self.N, self.T, self.L))
        self.E_z_zT = np.zeros((self.N, self.T, self.L, self.L))
        self.E_z_z1T = np.zeros((self.N, self.T-1, self.L, self.L))


    #     self.mu_predict = np.zeros((self.N, self.T, self.L))
    #     self.Sigma_predict = np.zeros((self.N, self.T, self.L, self.L))
    #     self.initialize_states_prior()

    # '''
    # Initialize mu and v
    # '''   
    # def initialize_states_prior(self):
    #     for n in range(self.N):
    #         # set first time point using prior
    #         self.mu_predict[n,0] = self.mu_0
    #         self.Sigma_predict[n,0] = self.V_0
    #         # set remaining time points
    #         for t in range(1, self.T):
    #             self.mu_predict[n,t] = np.matmul(self.A, self.mu_predict[n,t-1])
    #             self.Sigma_predict[n,t] = self.state_Sigma

    '''
    Run EM algo for given number of iterations
    '''
    def run_EM(self, iterations):
        for iteration in range(iterations):
            state_params, obs_params, ll = self.run_EM_iteration()
            print(ll)

    '''
    Full pass of EM algo
    '''    
    def run_EM_iteration(self):
        self.E_step()
        self._expectations()
        input(self.E_z_z1T)
        self.M_step()
        self.sample_states()
        self.log_likelihood()

    '''
    E step: estimate the posterior using forward and backward path given parameters
    '''
    def E_step(self):
        # forward path is estimating prediction parameters
        self._forward_path()
        # backward path estimate the smoothing parameters
        self._backward_path()
        
    '''
    M step: Update the parameters by maximizing the complete data 
        log likelihood with resprect to the posterior distribution
    '''
    def M_step(self):
       # run M_step tp update the kalman parameters
        self._update_W()
        self._update_obs_Sigma()
        self._update_A()
        self._update_state_Sigma()
        self._update_initials()

    '''
    E_step helper: Get filtered posterior: filtered_mu and filtered_V
    '''      
    def _forward_path(self):
        Kalman_gain = np.zeros((self.N, self.T, self.L, self.P))
        for n in range(self.N):
            # Initialize
            self.filtered_mu[n,0] = self.mu_0
            self.filtered_V[n,0] = self.V_0
            # Forward
            for t in range(1, self.T):
                ## Prediction Step
                # eq:invariant_filteringMut-1 Bishop 13.89 Murphy 18.27
                self.predicted_mu[n,t] = np.matmul(self.A, self.filtered_mu[n,t-1])
                # eq:invariant_filteringVt-1 Bishop 13.88, Murphy 18.28
                self.predicted_V[n,t] = (np.matmul(np.matmul(self.A, self.filtered_V[n, t-1]), self.A.T) 
                                        + self.state_Sigma)
                ## Measurement step
                # eq:invariant_kalman_gain2 Bishop 13.92 Murphy 18.39
                Kalman_gain[n,t] = np.matmul(np.matmul(self.predicted_V[n,t], self.W.T), 
                                    inv(np.matmul(np.matmul(self.W, self.predicted_V[n,t]), self.W.T) 
                                    + self.obs_Sigma))
                # eq:invariant_residual Murphy 18.33
                residual = self.X[n,t] - np.matmul(self.W, self.predicted_mu[n,t])
                # eq:invariant_filteringMu Bishop 13.89 Murphy 18.31
                self.filtered_mu[n,t] = self.predicted_mu[n,t] + np.matmul(Kalman_gain[n,t], residual)
                # eq:invariant_filteringV  Bishop 13.90 Murphy 18.32
                self.filtered_V[n,t] = np.matmul(np.eye(self.L) - np.matmul(Kalman_gain[n,t], self.W), 
                                    self.predicted_V[n,t])

    '''
    E_step helper: Get smoothed posterior
    '''
    def _backward_path(self):
        for n in range(self.N):
            # initialize 
            self.smoothed_mu[n,-1] = self.filtered_mu[n,-1]
            self.smoothed_V[n,-1] = self.filtered_V[n,-1]
            # backward
            for t in range(self.T-2, -1, -1): #-2 because indexing starts at 0
                self.backward_Kalman[n,t] = np.matmul(np.matmul(self.smoothed_V[n,t], self.A.T),
                                            inv(self.predicted_V[n,t+1]))
                # eq:invariant_smoothedMu Bishop 13.100 Murphy 18.57
                self.smoothed_mu[n,t] = self.filtered_mu[n,t] + np.matmul(self.backward_Kalman[n,t], 
                                        self.smoothed_mu[n,t+1] - self.predicted_mu[n,t+1])
                # eq:invariant_smoothedV Bishop 13.101 Murphy 15.58
                self.smoothed_V[n,t] = self.filtered_V[n,t] + np.matmul(self.backward_Kalman[n,t], 
                                    np.matmul((self.smoothed_V[n,t+1] - self.predicted_V[n,t+1]), 
                                    self.backward_Kalman[n,t].T))

    '''
    Get expections needed for M step
    '''
    def _expectations(self):
        # eq:Expectations Bishop 13.105-13.107
        self.E_z = self.smoothed_mu
        for n in range(self.N):
            self.E_z_zT[n] = [self.smoothed_V[n,t] + np.matmul(self.smoothed_mu[n,t], 
                            self.smoothed_mu[n,t].T) for t in range(self.T)]
            self.E_z_z1T[n] = [np.matmul(self.backward_Kalman[n,t-1], self.smoothed_V[n,t]) +\
                            np.matmul(self.smoothed_mu[n,t], self.smoothed_mu[n,t-1].T) \
                            for t in range(1, self.T)]

    '''
    Sample observation using states and observation statistics
    '''
    def sample_observation(self):
        # TODO
        pass

    '''
    Sample latent states using statistics
    '''
    def sample_states(self):
        self.z = [0] * self.T
        for t in range(self.T):
            self.z[t] = np.random.multivariate_normal(self.mu_predict[t], self.Sigma_predict[t])

    '''
    Calculate complete data log likelihood
    '''
    def log_likelihood(self):
        # eq:LLprior
        ll_prior = -self.L/2*np.log(2*np.pi) - 0.5*np.log(det(self.V_0)) - \
                   0.5*np.matmul(np.expand_dims(self.z[0]-self.mu_0, 1).T,
                   np.matmul(inv(self.V_0), np.expand_dims(self.z[0]-self.mu_0, 1)))
        # eq:LLobs         
        ll_obs = -((self.T-1)*self.L)/2  - (self.T/2)*np.log(det(self.obs_Sigma)) -\
                sum([0.5*np.matmul(np.expand_dims(self.z[t]-np.matmul(self.W, self.z[t]), 1).T,
                np.matmul(inv(self.obs_Sigma),np.expand_dims(self.z[t]-np.matmul(self.W,self.z[t]), 1)))
                for t in range(self.T)])
        # eq:LLstate
        ll_state = -(self.T*self.P)/2 - ((self.T-1)/2)*np.log(det(self.state_Sigma)) -\
                    sum([0.5*np.matmul(np.expand_dims(self.z[t]-np.matmul(self.A, self.z[t-1]), 1).T,
                    np.matmul(inv(self.state_Sigma), np.expand_dims(self.z[t]-np.matmul(self.A, self.z[t-1]), 1)))
                    for t in range(1, self.T)])
        # eq:completeLL
        self.ll = ll_prior + ll_state + ll_obs

    '''
    M_step helper: update observation matrix
    '''
    def _update_W(self):
        # TODO
        # self.W = 
        pass

    '''
    M_step helper: update observation Sigma
    '''
    def _update_obs_Sigma(self):
        # TODO
        # self.obs_Sigma = 
        pass

    '''
    M_step helper: update transition matrix
    '''
    def _update_A(self):
        # TODO
        # self.A = 
        pass

    '''
    M_step helper: update latent Sigma
    '''
    def _update_state_Sigma(self):
        # TODO
        # self.state_Sigma
        pass
    
    '''
    M_step helper: update inital mu and v
    '''   
    def _update_initials(self):
        # TODO
        # self.mu_0 = 
        # self.V_0 = 
        pass
    

