from timeit import default_timer as timer
import torch
import warnings
import numpy as np
from scipy import linalg
from utils import *

VERBOSE=1
DEVICE='cuda:0'
# torch.set_default_dtype(torch.float64)

def _last_dims(X, t, ndims=2):
    if len(X.size()) == ndims + 1:
        return X[t]
    elif len(X.size()) == ndims:
        return X
    else:
        raise ValueError(("X only has %d dimensions when %d" +
                " or more are required") % (len(X.size()), ndims))


def _loglikelihoods(observation_matrices, observation_offsets, observation_covariance, predicted_state_means, predicted_state_covariances, observations, observations_mask):
    n_samples = observations.shape[0]
    n_timesteps = observations.shape[1]
    loglikelihoods = torch.zeros(n_samples, n_timesteps).to(DEVICE)
    for n in range(n_samples):
        for t in range(n_timesteps):
            observation = observations[n,t]
            observation_mask = observations_mask[n, t]
            if not np.any(observation_mask):
                observation_matrix = _last_dims(observation_matrices, t)
                observation_offset = _last_dims(observation_offsets, t, ndims=1)
                predicted_state_mean = _last_dims(predicted_state_means[n], t, ndims=1)
                predicted_state_covariance = _last_dims(predicted_state_covariances, t)

                predicted_observation_mean = (observation_matrix @ predicted_state_mean) + observation_offset
                predicted_observation_covariance = (observation_matrix @ (predicted_state_covariance @ observation_matrix.T)) + observation_covariance
                
                loglikelihoods[n, t] = log_multivariate_normal_density(observation[None, :], predicted_observation_mean[None, :], predicted_observation_covariance[None, :, :])
                                                                        
    return loglikelihoods
    # N X T



def _filter_predict(transition_matrix, transition_covariance, transition_offset, current_state_mean, current_state_covariance):
    # TMa = L X L 
    # CSMean = L,
    # TO = L, 
    # PredSMean = L,
    # PredSCov = L X L
    if VERBOSE==3:
        print('~~~~~~~Filter Prediction .......')
    predicted_state_mean = (transition_matrix@current_state_mean) + transition_offset

    predicted_state_covariance = (transition_matrix @ (current_state_covariance @ transition_matrix.T)) + transition_covariance

    return (predicted_state_mean, predicted_state_covariance)
    # L,     LXL


def _filter_correct(observation_matrix, observation_covariance, observation_offset, predicted_state_mean, predicted_state_covariance, observation, observation_mask):
    # ObMa: dM X L
    # obCov: dM X dM
    # obOff: dM, 
    # PredSMean: L, 
    # PredSCov: L X L
    # Ob: dM,
    # ObMask: dM, 
    if VERBOSE == 3:
        print('~~~~~~~Filter Correction .......')
    if not np.any(observation_mask):
        # Input is not masked
        predicted_observation_mean = observation_matrix @ predicted_state_mean + observation_offset # dM, 
        
        predicted_observation_covariance = (observation_matrix @ (predicted_state_covariance @ observation_matrix.T)) + observation_covariance # dM X dM
        
        try:
            pred_ob_c_inv = torch.linalg.pinv(predicted_observation_covariance)
        except:
            print(f'inside filer correct exception {torch.sum(predicted_observation_covariance)}')
            predicted_observation_covariance_pd = nearestPD(predicted_observation_covariance)
            pred_ob_c_inv = torch.linalg.pinv(predicted_observation_covariance_pd, hermitian=True)
            # raise RuntimeError('Inversion of Predicted Observation Covariance FAILED')
        kalman_gain = (predicted_state_covariance @ (observation_matrix.T @ pred_ob_c_inv)) # L X dM

        corrected_state_mean = predicted_state_mean + (kalman_gain @ (observation - predicted_observation_mean)) # L, 
        
        # corrected_state_covariance = predicted_state_covariance - (kalman_gain @ (observation_matrix @ predicted_state_covariance)) # L X L
        # Joseph form - reduces risk of neg diag values
        temp_matrix = torch.eye(predicted_state_covariance.size()[0]).to(DEVICE) - (kalman_gain @ observation_matrix) # L x L
        corrected_state_covariance = (temp_matrix @ (predicted_state_covariance @ temp_matrix.T)) + (kalman_gain @ (observation_covariance @ kalman_gain.T))
            
    else:
        # Observation is masked
        n_dim_state = predicted_state_covariance.size()[0]
        n_dim_obs = observation_matrix.size()[0]
        kalman_gain = torch.zeros(n_dim_state, n_dim_obs).to(DEVICE)
        corrected_state_mean = predicted_state_mean
        corrected_state_covariance = predicted_state_covariance

    return (kalman_gain, corrected_state_mean, corrected_state_covariance)
    # L X dM,      L,    L X L


def _filter(transition_matrices, observation_matrices, transition_covariance, observation_covariance, transition_offsets, observation_offsets, initial_state_mean, initial_state_covariance, observations, observations_mask):
    # TrMat: T-1, L X L
    # ObMat: T, dM X L
    # TrCov: L X L
    # ObCov: dM X dM
    # TrOffsets: L,
    # ObOffsets: dM, 
    # InStateMean: L,
    # InStCov: L X L
    # Observations: N, T, dM
    n_dim_state = len(initial_state_mean)
    n_samples = observations.size()[0]
    n_timesteps = observations.size()[1]
    n_dim_obs = observations.size()[2]

    predicted_state_means = torch.zeros(n_samples, n_timesteps, n_dim_state).to(DEVICE)
    predicted_state_covariances = torch.zeros((n_timesteps, n_dim_state, n_dim_state)).to(DEVICE)
    kalman_gains = torch.zeros(n_timesteps, n_dim_state, n_dim_obs).to(DEVICE)
    filtered_state_means = torch.zeros(n_samples, n_timesteps, n_dim_state).to(DEVICE)
    filtered_state_covariances = torch.zeros(n_timesteps, n_dim_state, n_dim_state).to(DEVICE)

    # TODO: take sample invariant quantities out of loop so it runs faster
    if VERBOSE == 1:
        print(f'----Filtering step-------')
    start_f_time = timer()
    for n in range(n_samples):
        for t in range(n_timesteps):
            if t == 0:
                predicted_state_means[t] = initial_state_mean
                predicted_state_covariances[t] = initial_state_covariance
            else:
                transition_matrix = _last_dims(transition_matrices, t - 1)
                transition_covariance = _last_dims(transition_covariance, t - 1)
                transition_offset = _last_dims(transition_offsets, t - 1, ndims=1)
                predicted_state_means[n, t], predicted_state_covariances[t] = (_filter_predict(transition_matrix, transition_covariance, transition_offset,
                                                                                                filtered_state_means[n, t - 1], filtered_state_covariances[t - 1]))
            observation_matrix = _last_dims(observation_matrices, t) # dM X L
            observation_covariance = _last_dims(observation_covariance, t) # dM X dM 
            observation_offset = _last_dims(observation_offsets, t, ndims=1) # dM
            (kalman_gains[t], filtered_state_means[n, t], filtered_state_covariances[t]) = (_filter_correct(observation_matrix, observation_covariance, observation_offset,
                                                                                                            predicted_state_means[n, t], predicted_state_covariances[t], observations[n, t], observations_mask[n, t]))
    end_f_time = timer()
    if VERBOSE == 1:
        print(f'*** Filtering step done in {(end_f_time - start_f_time)} seconds ***')
    return (predicted_state_means, predicted_state_covariances, kalman_gains, filtered_state_means, filtered_state_covariances)

def _smooth_update(transition_matrix, filtered_state_mean, filtered_state_covariance, predicted_state_mean, predicted_state_covariance, next_smoothed_state_mean, next_smoothed_state_covariance):
    # TrMat: L X L
    # FiltStMean: L
    # FiltStCov: L X L
    # PredStMean: L,
    # PredStCov: L X L
    # NextSmMean: L,
    # NextStCov: L X L
    if VERBOSE==3:
        print('~~~~~~~Smooth Updating .......')
    try:
        pred_st_cov_inv = torch.linalg.pinv(predicted_state_covariance)
    except:
        pred_st_cov_pd = nearestPD(predicted_state_covariance)
        pred_st_cov_inv = torch.linalg.pinv(pred_st_cov_pd, hermitian=True)
        # raise RuntimeError('Inversion of Predicted State Covariance failed')
    kalman_smoothing_gain = (filtered_state_covariance @ (transition_matrix.T @ pred_st_cov_inv)) # L X L

    smoothed_state_mean = filtered_state_mean + (kalman_smoothing_gain @ (next_smoothed_state_mean - predicted_state_mean)) # L, 
    smoothed_state_covariance = filtered_state_covariance + (kalman_smoothing_gain @ ((next_smoothed_state_covariance - predicted_state_covariance) @ kalman_smoothing_gain.T)) # L X L
    
    return (smoothed_state_mean, smoothed_state_covariance, kalman_smoothing_gain)
    # L,    L X L,     L X L


def _smooth(transition_matrices, filtered_state_means, filtered_state_covariances, predicted_state_means, predicted_state_covariances):
    # TrMat: T-1, L X L
    # FiltStMeans: N, T, L
    # FiltStCov: T, L X L
    # PredStMeans: N, T, L
    # PredStCov: T, L X L
    if VERBOSE==3:
        print('~~~~~~~Smoothing .......')
    n_samples, n_timesteps, n_dim_state = filtered_state_means.size()
    smoothed_state_means = torch.zeros(n_samples, n_timesteps, n_dim_state).to(DEVICE) # return this
    smoothed_state_covariances = torch.zeros(n_timesteps, n_dim_state, n_dim_state).to(DEVICE) # return this
    kalman_smoothing_gains = torch.zeros(n_timesteps - 1, n_dim_state, n_dim_state).to(DEVICE) # return this

    smoothed_state_means[:, -1] = filtered_state_means[:, -1]
    smoothed_state_covariances[-1] = filtered_state_covariances[-1]

    # TODO: take sample invariant quanities out of loop so it runs faster
    for n in range(n_samples):
        for t in reversed(range(n_timesteps - 1)):
            transition_matrix = _last_dims(transition_matrices, t)
            (smoothed_state_means[n, t], smoothed_state_covariances[t], kalman_smoothing_gains[t]) = (_smooth_update(transition_matrix, filtered_state_means[n, t], filtered_state_covariances[t], 
                                                            predicted_state_means[n, t + 1], predicted_state_covariances[t + 1], smoothed_state_means[n, t + 1],smoothed_state_covariances[t + 1]))
    return (smoothed_state_means, smoothed_state_covariances, kalman_smoothing_gains)
    # N X T X L,        T X L X L,      T-1 X L X L


def _smooth_pair(smoothed_state_covariances, kalman_smoothing_gain):
    # SmSCov: T X L X L
    # KSmGain: T-1 X L X L
    if VERBOSE==3:
        print('~~~~~~~~ Smooth Pairing.......')
    n_timesteps, n_dim_state, _ = smoothed_state_covariances.size()
    pairwise_covariances = torch.zeros(n_timesteps, n_dim_state, n_dim_state).to(DEVICE)
    for t in range(1, n_timesteps):
        pairwise_covariances[t] = smoothed_state_covariances[t] @ kalman_smoothing_gain[t - 1].T
    return pairwise_covariances
    # T X L X L

def _em_observation_matrix(observations, observation_offsets, smoothed_state_means, smoothed_state_covariances, observations_mask):
    _, _, n_dim_state = smoothed_state_means.size()
    n_samples, n_timesteps, n_dim_obs = observations.size()
    res1 = torch.zeros(n_dim_obs, n_dim_state).to(DEVICE) # dM X L
    res2 = torch.zeros(n_dim_state, n_dim_state).to(DEVICE) # L X L
    for n in range(n_samples):
        for t in range(n_timesteps):
            if not np.any(observations_mask[n, t]):
                # if not masked
                observation_offset = _last_dims(observation_offsets, t, ndims=1)
                res1 += torch.outer(observations[n,t] - observation_offset, smoothed_state_means[n, t])
                res2 += smoothed_state_covariances[t] + torch.outer(smoothed_state_means[n, t], smoothed_state_means[n, t])
    try:
        res2_inv = torch.linalg.pinv(res2)
    except:
        res2_pd = nearestPD(res2)
        res2_inv = torch.linalg.pinv(res2_pd, hermitian=True)
        # raise RuntimeError('Inverse in EM Obs Matrix Failed')
    return res1 @ res2_inv
    # dM X L


def _em_observation_covariance(observations, observation_offsets, transition_matrices, smoothed_state_means, smoothed_state_covariances, observations_mask=None):

    _, _, n_dim_state = smoothed_state_means.size()
    n_samples, n_timesteps, n_dim_obs = observations.size()
    res = torch.zeros(n_dim_obs, n_dim_obs).to(DEVICE) # dm X dM
    n_obs = 0
    for n in range(n_samples):
        for t in range(n_timesteps):
            if not np.any(observations_mask[n, t]):
                # if not masked input
                transition_matrix = _last_dims(transition_matrices, t) # dM X L SEE difference in initialization
                transition_offset = _last_dims(observation_offsets, t, ndims=1) # dM, 
                err = observations[n, t] - (transition_matrix @ smoothed_state_means[n, t]) - transition_offset # dM
                res += (torch.outer(err, err) + (transition_matrix @ (smoothed_state_covariances[t] @ transition_matrix.T))) # dM X dM
                n_obs += 1
    if n_obs > 0:
        return (1.0 / n_obs) * res
    else:
        return res
    # dM X dM 


def _em_transition_covariance(transition_matrices, transition_offsets, smoothed_state_means, smoothed_state_covariances, pairwise_covariances):
    n_timesteps, n_dim_state, _ = smoothed_state_covariances.size()
    n_samples = smoothed_state_means.size()[0]
    res = torch.zeros(n_dim_state, n_dim_state).to(DEVICE)
    for n in range(n_samples):
        for t in range(n_timesteps - 1):
            transition_matrix = _last_dims(transition_matrices, t)
            transition_offset = _last_dims(transition_offsets, t, ndims=1)
            err = smoothed_state_means[n, t + 1] - (transition_matrix @ smoothed_state_means[n, t]) - transition_offset # L, 
            
            Vt1t_A = pairwise_covariances[t + 1] @ transition_matrix.T # 
            res += (torch.outer(err, err) + (transition_matrix @ (smoothed_state_covariances[t] @ transition_matrix.T)) + smoothed_state_covariances[t + 1] - Vt1t_A - Vt1t_A.T) # L X L 

    return (1.0 / (n_samples*(n_timesteps - 1))) * res
    # L X L


def _em_initial_state_mean(smoothed_state_means):
    return torch.mean(smoothed_state_means[:,0,:], 0)
    # L, 


def _em_initial_state_covariance(initial_state_mean, smoothed_state_means, smoothed_state_covariances):
    n_samples = smoothed_state_means.size()[0]
    # summation = 0
    L = smoothed_state_means.size()[-1]
    summation = torch.zeros(L, L).to(DEVICE)
    for n in range(n_samples):
        x0 = smoothed_state_means[n, 0] # L, 
        x0_x0 = smoothed_state_covariances[0] + torch.outer(x0, x0) # L X L
        summation += (x0_x0 - torch.outer(initial_state_mean, x0) - torch.outer(x0, initial_state_mean) + torch.outer(initial_state_mean, initial_state_mean)) # L X L
    return (1/n_samples)*summation
    # L X L
        

def _em_transition_offset(transition_matrices, smoothed_state_means):
    n_samples, n_timesteps, n_dim_state = smoothed_state_means.size()
    transition_offset = torch.zeros(n_dim_state).to(DEVICE) # L, 
    for n in range(n_samples):
        for t in range(1, n_timesteps):
            transition_matrix = _last_dims(transition_matrices, t - 1)
            transition_offset += (smoothed_state_means[n, t] - (transition_matrix @ smoothed_state_means[n, t - 1]))
    if n_timesteps > 1:
        return (1.0 / (n_samples*(n_timesteps - 1))) * transition_offset
    else:
        return torch.zeros(n_dim_state).to(DEVICE)
    # L, 


def _em_observation_offset(observation_matrices, smoothed_state_means, observations, observations_mask):
    n_samples, n_timesteps, n_dim_obs = observations.shape
    observation_offset = torch.zeros(n_dim_obs).to(DEVICE) # dM, 
    n_obs = 0
    for n in range(n_samples):
        for t in range(n_timesteps):
            if not np.any(observations_mask[n,t]):
                observation_matrix = _last_dims(observation_matrices, t)
                observation_offset += (observations[n,t] - (observation_matrix @ smoothed_state_means[n,t]))
                n_obs += 1
    if n_obs > 0:
        return (1.0 / n_obs) * observation_offset
    else:
        return observation_offset
    # dM, 

def _time_varying_em(observations, transition_offsets, observation_offsets, smoothed_state_means, smoothed_state_covariances, pairwise_covariances, given={}, observations_mask=None):
    
    if 'observation_matrices' in given:
        observation_matrix = given['observation_matrices']
    else:
        observation_matrix = _em_observation_matrix(observations, observation_offsets, smoothed_state_means, smoothed_state_covariances, observations_mask) # dM X L 

    if 'observation_covariance' in given:
        observation_covariance = given['observation_covariance']
    else:
        observation_covariance = _em_observation_covariance(observations, observation_offsets, observation_matrix, smoothed_state_means, smoothed_state_covariances, observations_mask) # dM X dM

    if 'transition_matrices' in given:
        transition_matrix = given['transition_matrices']
    else:
        transition_matrix = _time_varying_em_transition_matrix(transition_offsets, smoothed_state_means,smoothed_state_covariances, pairwise_covariances) # T X L X L 

    if 'transition_covariance' in given:
        transition_covariance = given['transition_covariance']
    else:
        transition_covariance = _em_transition_covariance(transition_matrix, transition_offsets,smoothed_state_means, smoothed_state_covariances,pairwise_covariances) # L X L

    if 'initial_state_mean' in given:
        initial_state_mean = given['initial_state_mean']
    else:
        initial_state_mean = _em_initial_state_mean(smoothed_state_means) # L, 

    if 'initial_state_covariance' in given:
        initial_state_covariance = given['initial_state_covariance']
    else:
        initial_state_covariance = _em_initial_state_covariance(initial_state_mean, smoothed_state_means,smoothed_state_covariances) # L X L

    if 'transition_offsets' in given:
        transition_offset = given['transition_offsets']
    else:
        transition_offset = _em_transition_offset(transition_matrix,smoothed_state_means) # L, 

    if 'observation_offsets' in given:
        observation_offset = given['observation_offsets']
    else:
        observation_offset = _em_observation_offset(observation_matrix, smoothed_state_means, observations, observations_mask)

    return (transition_matrix, observation_matrix, transition_offset, observation_offset, transition_covariance, observation_covariance, initial_state_mean,initial_state_covariance)

def _time_varying_em_transition_matrix(transition_offsets, smoothed_state_means, smoothed_state_covariances, pairwise_covariances):
    n_timesteps, n_dim_state, _ = smoothed_state_covariances.size()
    n_samples = smoothed_state_means.size()[0]
    res1 = torch.zeros(n_timesteps, n_dim_state, n_dim_state).to(DEVICE)
    res2 = torch.zeros(n_timesteps, n_dim_state, n_dim_state).to(DEVICE)
    for n in range(n_samples):
        for t in range(1, n_timesteps):
            transition_offset = _last_dims(transition_offsets, t - 1, ndims=1)
            res1[t-1] += (pairwise_covariances[t] + torch.outer(smoothed_state_means[n, t], smoothed_state_means[n, t - 1]) - torch.outer(transition_offset, smoothed_state_means[n, t - 1]))
            res2[t-1] += (smoothed_state_covariances[t - 1] + torch.outer(smoothed_state_means[n, t - 1], smoothed_state_means[n, t - 1]))
    updated_matrix = torch.zeros(n_timesteps, n_dim_state, n_dim_state).to(DEVICE)
    for t in range(n_timesteps):
        try:
            res2_t_inv = torch.linalg.pinv(res2[t]) # L X L 
        except:
            res2_t_pd = nearestPD(res2[t])
            res2_t_inv = torch.linalg.pinv(res2_t_pd, hermitian=True) # L X L 
            # raise RuntimeError('Inversion Failed in Time Varying EM Tranisition Matrix')
        updated_matrix[t] = res1[t] @ res2_t_inv
    return updated_matrix 
    # T X L X L


class LDS(object):
    def __init__(self, transition_matrices=None, observation_matrices=None,
            em_vars=['transition_covariance', 'observation_covariance',
                     'initial_state_mean', 'initial_state_covariance'], 
            n_dim_state=None, n_dim_obs=None, device='cuda:0') -> None:
        self.n_dim_state = n_dim_state
        self.n_dim_obs = n_dim_obs
        self.device = device

        self.transition_matrices = torch.from_numpy(transition_matrices).float().to(device) # initialized with initial_A of shape (T-1, L, L)
        self.observation_matrices = torch.from_numpy(observation_matrices).float().to(device) # initialized with initial_W of shape (T, dM, L)

        self.transition_covariance = torch.eye(self.n_dim_state).to(self.device)
        self.observation_covariance = torch.eye(self.n_dim_obs).to(self.device)

        self.transition_offsets = torch.zeros(self.n_dim_state).to(self.device)
        self.observation_offsets = torch.zeros(self.n_dim_obs).to(self.device)

        self.initial_state_mean = torch.zeros(self.n_dim_state).to(self.device)
        self.initial_state_covariance = torch.eye(self.n_dim_state).to(self.device)

        self.random_state = 0
        self.em_vars = em_vars

    def sample(self, n_timesteps, initial_state=None, random_state=None):
        # initialize parameters
        transition_matrices = self.transition_matrices 
        transition_offsets = self.transition_offsets
        transition_covariance = self.transition_covariance
        observation_matrices = self.observation_matrices
        observation_offsets = self.observation_offsets
        observation_covariance = self.observation_covariance
        initial_state_mean = self.initial_state_mean
        initial_state_covariance = self.initial_state_covariance

        n_dim_state = transition_matrices.size()[-2]
        n_dim_obs = observation_matrices.size()[-2]
        states = torch.zeros(n_timesteps, n_dim_state).to(DEVICE)
        observations = torch.zeros(n_timesteps, n_dim_obs).to(DEVICE)

        # logic for selecting initial state
        if initial_state is None:
            initial_state = torch.distributions.multivariate_normal.MultivariateNormal(initial_state_mean, initial_state_covariance).sample()

        # logic for generating samples
        for t in range(n_timesteps):
            if t == 0:
                states[t] = initial_state
            else:
                transition_matrix = _last_dims(transition_matrices, t - 1)
                transition_offset = _last_dims(transition_offsets, t - 1, ndims=1)
                transition_covariance = _last_dims(transition_covariance, t - 1)
                states[t] = (transition_matrix @ states[t - 1]) + transition_offset + torch.distributions.multivariate_normal.MultivariateNormal(torch.zeros(n_dim_state).to(DEVICE), transition_covariance).sample()

            observation_matrix = _last_dims(observation_matrices, t)
            observation_offset = _last_dims(observation_offsets, t, ndims=1)
            observation_covariance = _last_dims(observation_covariance, t)
            observations[t] = (observation_matrix @ states[t]) + observation_offset + torch.distributions.multivariate_normal.MultivariateNormal(torch.zeros(n_dim_obs).to(DEVICE), observation_covariance).sample()

        return (states, observations)

    # def predict(self, partial_observations, n_timesteps, initial_state=None, random_state=None):
    #     # initialize parameters
    #     transition_matrices = self.transition_matrices 
    #     transition_offsets = self.transition_offsets
    #     transition_covariance = self.transition_covariance
    #     observation_matrices = self.observation_matrices
    #     observation_offsets = self.observation_offsets
    #     observation_covariance = self.observation_covariance
    #     initial_state_mean = self.initial_state_mean
    #     initial_state_covariance = self.initial_state_covariance

    #     n_dim_state = transition_matrices.size()[-2]
    #     n_dim_obs = observation_matrices.size()[-2]
    #     n_samples = partial_observations.size()[0]
    #     n_partial = partial_observations.size()[1]
    #     partial_states = self.smooth(partial_observations)[0] # TODO check mask here
    #     states = torch.zeros(n_samples, n_timesteps, n_dim_state).to(DEVICE)
    #     states[:, :n_partial] = partial_states
    #     observations = torch.zeros(n_samples, n_timesteps, n_dim_obs).to(DEVICE)
    #     observations[:, :n_partial] = partial_observations

    #     # logic for selecting initial state
    #     if initial_state is None:
    #         initial_state = torch.distributions.multivariate_normal.MultivariateNormal(initial_state_mean, initial_state_covariance).sample()

    #     # logic for generating samples
    #     for n in range(n_samples):
    #         for t in range(n_partial, n_timesteps):
    #             if t == 0:
    #                 states[n,t] = initial_state
    #             else:
    #                 transition_matrix = _last_dims(transition_matrices, t - 1)
    #                 transition_offset = _last_dims(transition_offsets, t - 1, ndims=1)
    #                 transition_covariance = _last_dims(transition_covariance, t - 1)
    #                 states[n,t] = (transition_matrix @ states[n,t - 1]) + transition_offset + torch.distributions.multivariate_normal.MultivariateNormal(torch.zeros(n_dim_state).to(DEVICE), transition_covariance).sample()

    #             observation_matrix = _last_dims(observation_matrices, t)
    #             observation_offset = _last_dims(observation_offsets, t, ndims=1)
    #             observation_covariance = _last_dims(observation_covariance, t)
    #             observations[n, t] = (observation_matrix @ states[n, t])+ observation_offset + torch.distributions.multivariate_normal.MultivariateNormal(torch.zeros(n_dim_obs).to(DEVICE), observation_covariance).sample()

    #     return (states, observations)

    def reconstruct(self, observations, random_state=None, observations_mask=None):
        # initialize parameters
        transition_matrices = self.transition_matrices 
        transition_offsets = self.transition_offsets
        transition_covariance = self.transition_covariance
        observation_matrices = self.observation_matrices
        observation_offsets = self.observation_offsets
        observation_covariance = self.observation_covariance
        initial_state_mean = self.initial_state_mean
        initial_state_covariance = self.initial_state_covariance

        # Fitting state
        (predicted_state_means, predicted_state_covariances, kalman_gains, filtered_state_means, filtered_state_covariances) = (_filter(self.transition_matrices, self.observation_matrices,
                                                                                                                                self.transition_covariance, self.observation_covariance,
                                                                                                                                self.transition_offsets, self.observation_offsets,
                                                                                                                                self.initial_state_mean, self.initial_state_covariance,
                                                                                                                                observations, observations_mask))
        (smoothed_state_means, smoothed_state_covariances, kalman_smoothing_gains) = (_smooth(self.transition_matrices, filtered_state_means, filtered_state_covariances, predicted_state_means, predicted_state_covariances))
        n_samples = observations.size()[0]
        n_timesteps = observations.size()[1]
        n_dim_obs = observations.size()[2]
        reconstructed = torch.zeros(n_samples, n_timesteps, n_dim_obs).to(DEVICE)

        # Reconstructing observation from state
        for n in range(n_samples):
            for t in range(n_timesteps):
                observation_matrix = _last_dims(observation_matrices, t)
                observation_offset = _last_dims(observation_offsets, t, ndims=1)
                observation_covariance = _last_dims(observation_covariance, t)
                reconstructed[n, t] = (observation_matrix @ smoothed_state_means[n, t]) + observation_offset
        return reconstructed

    # def smooth(self, X, observations_mask):
    #     # initialize parameters
    #     transition_matrices = self.transition_matrices 
    #     transition_offsets = self.transition_offsets
    #     transition_covariance = self.transition_covariance
    #     observation_matrices = self.observation_matrices
    #     observation_offsets = self.observation_offsets
    #     observation_covariance = self.observation_covariance
    #     initial_state_mean = self.initial_state_mean
    #     initial_state_covariance = self.initial_state_covariance

    #     (predicted_state_means, predicted_state_covariances, _, filtered_state_means, filtered_state_covariances) = (_filter(transition_matrices, observation_matrices, transition_covariance, observation_covariance,
    #                                                                                                      transition_offsets, observation_offsets, initial_state_mean, initial_state_covariance, X, observations_mask))
    #     (smoothed_state_means, smoothed_state_covariances) = (_smooth(transition_matrices, filtered_state_means, filtered_state_covariances, predicted_state_means, predicted_state_covariances)[:2])
        
    #     return (smoothed_state_means, smoothed_state_covariances)
    #     # L,       L X  L 

    def em(self, X, y=None, n_iter=10, em_vars=None, observations_mask=None):

        # Parameters already initialized
        # Create dictionary of variables not to perform EM on
        if em_vars is None:
            em_vars = self.em_vars
        if em_vars == 'all':
            given = {}
        else:
            given = {
                'transition_matrices': self.transition_matrices,
                'observation_matrices': self.observation_matrices,
                'transition_offsets': self.transition_offsets,
                'observation_offsets': self.observation_offsets,
                'transition_covariance': self.transition_covariance,
                'observation_covariance': self.observation_covariance,
                'initial_state_mean': self.initial_state_mean,
                'initial_state_covariance': self.initial_state_covariance
            }
            em_vars = set(em_vars)
            for k in list(given.keys()):
                if k in em_vars:
                    # Remove em vars from given
                    given.pop(k)

        # Check if a parameter is time varying
        time_varying = True
        for i in range(n_iter):
            (predicted_state_means, predicted_state_covariances, kalman_gains, filtered_state_means, filtered_state_covariances) = (_filter(self.transition_matrices, self.observation_matrices,
                                                                                                                                    self.transition_covariance, self.observation_covariance,
                                                                                                                                    self.transition_offsets, self.observation_offsets,
                                                                                                                                    self.initial_state_mean, self.initial_state_covariance,
                                                                                                                                    X, observations_mask))
            (smoothed_state_means, smoothed_state_covariances, kalman_smoothing_gains) = (_smooth(self.transition_matrices, filtered_state_means, filtered_state_covariances, predicted_state_means, predicted_state_covariances))
            sigma_pair_smooth = _smooth_pair(smoothed_state_covariances, kalman_smoothing_gains)
            if time_varying:
                (self.transition_matrices,  self.observation_matrices, self.transition_offsets, self.observation_offsets,
                 self.transition_covariance, self.observation_covariance, self.initial_state_mean, self.initial_state_covariance) = (_time_varying_em(X, self.transition_offsets,
                                                                            self.observation_offsets, smoothed_state_means, smoothed_state_covariances, sigma_pair_smooth, given=given, observations_mask=observations_mask))
        return self

    def loglikelihood(self, X, observations_mask):
        # initialize parameters
        transition_matrices = self.transition_matrices 
        transition_offsets = self.transition_offsets
        transition_covariance = self.transition_covariance
        observation_matrices = self.observation_matrices
        observation_offsets = self.observation_offsets
        observation_covariance = self.observation_covariance
        initial_state_mean = self.initial_state_mean
        initial_state_covariance = self.initial_state_covariance
        

        # apply the Kalman Filter
        (predicted_state_means, predicted_state_covariances, kalman_gains, filtered_state_means, filtered_state_covariances) = _filter(transition_matrices, observation_matrices, transition_covariance, observation_covariance,
                                                                                                                             transition_offsets, observation_offsets, initial_state_mean, initial_state_covariance, X, observations_mask)

        # get likelihoods for each time step
        loglikelihoods = _loglikelihoods(observation_matrices, observation_offsets, observation_covariance, predicted_state_means, predicted_state_covariances, X, observations_mask)

        return torch.sum(torch.mean(loglikelihoods, 0))