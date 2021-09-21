import numpy as np
from numpy.linalg import inv as inv
from numpy.linalg import det as det

'''
LDS model class
TODO: update to handle data with shape (N,T,P)
Input:
    - data: with shape (T, P) where T is the number of time points and P is the dimension of the data
    - obs_params: class of observation params
    - state_params: class of state params
'''
class LDS():
    def __init__(self, data, obs_params=None, state_params=None, prior_initialize=False, random_initial=False):
        # observation data
        self.X = data
        
        # observation parameters
        self.W = obs_params.W
        self.obs_Sigma = obs_params.obs_Sigma
        
        # states parameters
        self.mu_0 = state_params.init_mu
        self.V_0 = state_params.init_V
        self.A = state_params.A
        self.latent_Sigma = state_params.latent_Sigma

        # dimensions
        self.T, self.P = data.shape
        self.L = self.A.shape[0]
        
        # TODO: E-step parameters
            
        # initialize the latent states
        self.initialize_states_prior()
    
    '''
    Initialize mu and v
    TODO: update to handle data with shape (N,T,P)
    '''   
    def initialize_states_prior(self):
        self.mu_predict = [0] * self.T
        self.Sigma_predict = [0] * self.T

        self.mu_predict[0] = self.mu_0
        self.Sigma_predict[0] = self.V_0

        for t in range(1, self.T):
            self.mu_predict[t] = np.matmul(self.A, self.mu_predict[t-1])
            self.Sigma_predict[t] = self.latent_Sigma

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
        ll_state = -(self.T*self.P)/2 - ((self.T-1)/2)*np.log(det(self.latent_Sigma)) -\
                    sum([0.5*np.matmul(np.expand_dims(self.z[t]-np.matmul(self.A, self.z[t-1]), 1).T,
                    np.matmul(inv(self.latent_Sigma), np.expand_dims(self.z[t]-np.matmul(self.A, self.z[t-1]), 1)))
                    for t in range(1, self.T)])
        # eq:completeLL
        self.ll = ll_prior + ll_state + ll_obs

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
        self._update_Q()
        self._update_initials()

    '''
    E_step helper: Get filtered posterior
    '''      
    def _forward_path(self):
        # TODO
        pass
        
    '''
    E_step helper: Get smoothed posterior
    '''
    def _backward_path(self):
        # TODO
        pass

    '''
    Get expections needed for M step
    '''
    def _expectations(self):
       # TODO
       pass

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
    def _update_latent_Sigma(self):
        # TODO
        # self.latent_Sigma
        pass
    
    '''
    M_step helper: update inital mu and v
    '''   
    def _update_initials(self):
        # TODO
        # self.mu_0 = 
        # self.V_0 = 
        pass
    
    '''
    Full pass of EM algo
    '''    
    def run_EM_iteration(self):
        self.E_step()
        self._expectations()
        self.M_step()
        self.sample_states()
        self.log_likelihood()
        state_params = [self.mu_0, self.V_0, self.A, self.latent_Sigma, self.mu_predict, self.Sigma_predict]
        obs_params = [self.W, self.obs_Sigma]

        return [state_params, obs_params, self.ll]

'''
Observation parameters class
    TODO: add defaults
    - obs_matrix: is the PxL loading/observation-system matrix
    - obs_Sigma: is the PxP observation covariance matrix
'''
class obs_params():
    def __init__(self, obs_matrix, obs_Sigma):
        self.W = obs_matrix
        self.obs_Sigma = obs_Sigma

'''
State parameters class
    TODO: add defaults
    - trans_matrix: is the LxL transition matrix
    - latent_Sigma: is the LxL latent covariance matrix
    - init_mu: is the L latent prior mean
    - init_Sigma: is the LxL latent prior variance
'''
class state_params():
    def __init__(self, trans_matrix, latent_Sigma, init_mu, init_Sigma):
        self.A = trans_matrix
        self.latent_Sigma = latent_Sigma
        self.init_mu = init_mu
        self.init_V = init_Sigma

'''
Create default data for testing
TODO: add N
'''
class DefaultData():
    def __init__(self, T, P, L):
        self.T = T
        self.P = P
        self.L = L
        self.generate_states()
        self.generate_data()
    def generate_states(self):
        self.mu_0 = np.ones(self.L)
        self.V_0 = np.eye(self.L)
        self.A = 0.99*self.random_rotation(self.L, theta=45)
        self.latent_Sigma = 0.5*np.eye(self.L)
        self.C =  np.random.randn(self.P, self.L)
        self.obs_Sigma = 0.25*np.eye(self.P)

        self.mu = [0] * self.T
        self.sigma= [0] * self.T
        self.mu[0] = self.mu_0
        self.sigma[0] = self.sigma_0
        for t in range(1, self.T):
            self.mu[t] = np.matmul(self.A, self.mu[t-1])
            self.sigma[t] = self.latent_Sigma

        return self.mu, self.sigma

    '''
    Generate data using state and observation equations
    '''
    def generate_data(self):
        self.z = [0] * self.T
        self.epsilon = [0] * self.T
        self.data = np.zeros((self.T, self.p))
        for t in range(self.T):
            self.z[t] = np.random.multivariate_normal(self.mu[t], self.sigma[t])
            self.epsilon[t] = np.random.multivariate_normal(np.zeros(self.P), self.obs_Sigma)
            self.data[t, :] = np.matmul(self.W, self.z[t]) + self.epsilon[t]
        return self.data

    def log_likelihood(self):
        # TODOD: compute the log likelihood function

        ll_obs = sum([0.5*np.matmul(np.expand_dims(self.data[t]-np.matmul(self.C, self.h[t]), 1).T,
                                    np.matmul(inv(self.R),np.expand_dims(self.data[t]-np.matmul(self.C,self.h[t]), 1)))
                                    for t in range(self.T)]) + (self.T/2)*np.log(det(self.R))
        # print('llobs')
        # print(ll_obs)
        # print('Q')
        # print(self.Q)
        ll_state = sum([0.5*np.matmul(np.expand_dims(self.h[t]-np.matmul(self.A, self.h[t-1]), 1).T,
                                    np.matmul(inv(self.Q), np.expand_dims(self.h[t]-np.matmul(self.A, self.h[t-1]), 1)))
                                    for t in range(1, self.T)]) + ((self.T-1)/2)*np.log(det(self.Q))
        # print('state')
        # print(ll_state)
        ll_prior = 0.5*np.matmul(np.expand_dims(self.h[0]-self.mu_0, 1).T,
                                    np.matmul(inv(self.sigma_0), np.expand_dims(self.h[0]-self.mu_0, 1))) + \
                   0.5*np.log(det(self.sigma_0))

        self.ll = -ll_obs -ll_state-ll_prior-((self.T *(self.p+self.q))/2)*np.log(2*np.pi)

        return self.ll
    def random_rotation(self, n, theta=None):
        if theta is None:
            # Sample a random, slow rotation
            theta = 0.5 * np.pi * np.random.rand()

        if n == 1:
            return np.random.rand() * np.eye(1)

        rot = np.array([[np.cos(theta), -np.sin(theta)],
                        [np.sin(theta), np.cos(theta)]])
        out = np.zeros((n, n))
        out[:2, :2] = rot
        q = np.linalg.qr(np.random.randn(n, n))[0]
        return q.dot(out).dot(q.T)