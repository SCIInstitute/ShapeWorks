import math
import numpy as np
import matplotlib.pyplot as plt
np.random.seed(0)

import lds_model

'''
Input: N - number of samples, T - number of time points
Output: Generated data with shape (N, T, P) where P = dM (here d is 2 and M is 9)
'''
def generate_ellipse_data(N, T, plot=False):
    # Major radius random value between 2 and 4 
    # Minor radius varies between 1 and 2 on a sin wave dependent on t
    data = []
    if plot:
        plt_x = []
        plt_y = []
    angles = np.arange(0, 2*np.pi + np.pi/4, np.pi/4) # for 9 correspondences 
    for sample_index in range(N):
        if plot:
            plt_x.append([])
            plt_y.append([])
        data.append([])
        major_radius = np.random.random()*3 + 1
        x_values = major_radius*np.cos(angles)
        for i in range(T):
            minor_radius = np.sin(i*(np.pi/4)) + 2
            y_values = minor_radius*np.sin(angles)
            data[sample_index].append([x_values,y_values])
            if plot:
                plt_x[sample_index].append(x_values)
                plt_y[sample_index].append(y_values)
    data = np.asarray(data, np.float32)
    data = data.reshape(N, T, 2*len(angles))

    if plot:
        dim = math.ceil(T/8)
        fig, axs = plt.subplots(dim, 8)
        for ax in axs.flat:
            ax.set_xlim([-4, 4])
            ax.set_ylim([-4, 4])
            ax.label_outer()
        for i in range(len(plt_x)):
            row = -1
            for j in range(len(plt_x[0])):
                if j%8 == 0:
                    row +=1 
                axs[row][j%8].plot(plt_x[i][j], plt_y[i][j])
                axs[row][j%8].set_title('Time =' + str(j+1))
        plt.savefig("data_plot")

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
    lds = lds_model.LDS_Model(data_dim=P, latent_dim=L, num_time=T, num_samples=3)

    # Run EM
    print("Running EM")
    print("Joint expected log likelihood:")
    num_iterations = 3
    lds.run_EM(X, num_iterations)

    # Log prob
    log_probs = lds.data_log_prob(X)
    print("Resulting expected log likelihood of data:")
    print(np.mean(log_probs))