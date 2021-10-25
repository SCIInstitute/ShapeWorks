import lds_model
import numpy as np
import matplotlib.pyplot as plt
np.random.seed(0)

'''
Input: N - number of samples, T - number of time points
Output: Generated data with shape (N, T, P) where P = dM (here d is 2 and M is 9)
'''
def generate_ellipse_data(N, T):
    # Major radius random value between 2 and 4 
    # Minor radius varies between 1 and 2 on a sin wave dependent on t
    data = []
    angles = np.arange(0, 2*np.pi + np.pi/4, np.pi/4) # for 9 correspondences 
    for sample_index in range(N):
        data.append([])
        major_radius = np.random.random()*2 + 2
        x_values = major_radius*np.cos(angles)
        for i in range(T):
            minor_radius = np.sin(i*(np.pi/4)) + 2
            y_values = minor_radius*np.sin(angles)
            data[sample_index].append([x_values,y_values])
    data = np.array(data)
    data = data.reshape(N, T, 2*len(angles))
    return data

if __name__=="__main__":

	# Define dimensions
    N = 3    # number of samples
    d = 2    # dimension of correspondence points
    M = 9    # number of correspondence points
    L = 2    # dimension of latent space
    P = d*M  # dimension of observation space 
    T = 25   # number of time points

    # Generate data
    X = generate_ellipse_data(N,T)

    # Initialize LDS params to defaults
    obs_params = lds_model.Obs_params(P, L)
    state_params = lds_model.State_params(L)
    lds = lds_model.LDS_Model(X, obs_params, state_params)

    # Run EM
    num_iterations = 3
    # print(lds.mu_predict)
    lds.run_EM(num_iterations)
    # print(lds.mu_predict)
