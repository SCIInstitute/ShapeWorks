import torch
from Constants import *
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
from timeit import default_timer as timer
import LDS_model
import utils
from sklearn.preprocessing import MinMaxScaler
DEVICE = 'cuda:0'

class LDS_Analysis:
    def __init__(self, model_name, em_iterations, T, d, M, L, scaling=True) -> None:
        self.model_name = model_name
        self.out_dir = f'{SHAPE_MODELS_DIR}/{self.model_name}_LDS_Analysis_L_{L}/'
        if not os.path.exists(self.out_dir):
            os.makedirs(self.out_dir)
        self.particles_data = load_correspondences(shapes_desc_file=f'{PROJECT_DIR}/description.json', 
                                                correspondences_dir=f'{SHAPE_MODELS_DIR}/{self.model_name}_particles/',
                                                T=T, d=d, M=M)
        self.scaling_obj = MinMaxScaler()
        if scaling:
            self.scaled_particles_data = self.scaling_obj.fit_transform(self.particles_data.reshape(-1, self.particles_data.shape[-1])).reshape(self.particles_data.shape)
        else:
            self.scaled_particles_data = self.particles_data
        self.lds_model = self.build_lds_model(em_iterations, T, d, M, L, True)
    
    def build_lds_model(self, em_iterations, T=25, d=3, M=256, L=32, plot_likelihood=True):
        P = d * M
        initial_A = utils.repeat(T-1, np.eye(L))
        initial_W = utils.repeat(T, np.random.normal(0.0, 1.0, (P, L)))
        lds = LDS_model.LDS(n_dim_obs=P, n_dim_state=L,
                        transition_matrices = initial_A, 
                        observation_matrices = initial_W,
                        em_vars=[
                            'transition_matrices', 'observation_matrices',
                            'transition_covariance', 'observation_covariance',
                            'initial_state_mean', 'initial_state_covariance'
                        ])
        
        loglikelihoods = np.zeros(em_iterations)
        for i in range(len(loglikelihoods)):
            start_em = timer()
            print(f'-------Running EM iteration# {i} ----------')
            particles_tensor = torch.from_numpy(self.scaled_particles_data).float().to(DEVICE)
            particles_mask = np.zeros(self.particles_data.shape)
            lds = lds.em(particles_tensor, n_iter=1, observations_mask=particles_mask)
            end_em = timer()
            print(f'^^EM Done in {(end_em - start_em)/60} minutes, Computing Log-Likelihood ^^')
            start_log = timer()
            loglikelihoods[i] = lds.loglikelihood(particles_tensor, observations_mask=particles_mask)
            end_log = timer()
            print(f'Iteration {i}, log-likelihood = {loglikelihoods[i]} computed in {(end_log - start_log)/60} minutes \n\n')
        np.savtxt(f'{self.out_dir}/log_likelihood_{em_iterations}.txt', loglikelihoods)
        if plot_likelihood:
            plt.figure()
            plt.plot(loglikelihoods)
            plt.xlabel('EM Iteration')
            plt.ylabel('Average Log Likelihood')
            plt.savefig(f'{self.out_dir}/log_likelihood_{em_iterations}.png')
        return lds

print('new model l = 320')
analyze_cross_entropy_model = LDS_Analysis(model_name='pre_post_model_2500_64', em_iterations=100, T=25, d=3, M=64, L=8, scaling=False)