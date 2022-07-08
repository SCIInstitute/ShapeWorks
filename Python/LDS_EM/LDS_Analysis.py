import torch
from Constants import *
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
from timeit import default_timer as timer
from LDS_model import LDS
from utils import repeat
import random
from sklearn.preprocessing import MinMaxScaler
torch.set_default_dtype(torch.float64)

class LDS_Analysis:
    def __init__(self, model_name, em_iterations, T, d, M, L, scaling=True, train_test_split_ratio=0.8) -> None:
        self.model_name = model_name
        self.out_dir = f'{SHAPE_MODELS_DIR}/{self.model_name}_LDS_Analysis_L_{L}/'
        if not os.path.exists(self.out_dir):
            os.makedirs(self.out_dir)
        self.particles_data = load_correspondences(shapes_desc_file=f'{PROJECT_DIR}/description.json', 
                                                correspondences_dir=f'{SHAPE_MODELS_DIR}/{self.model_name}_particles/',
                                                T=T, d=d, M=M)
        self.train_test_split_ratio = train_test_split_ratio
        self.scaling_obj = MinMaxScaler()
        if scaling:
            self.scaled_particles_data = self.scaling_obj.fit_transform(self.particles_data.reshape(-1, self.particles_data.shape[-1])).reshape(self.particles_data.shape)
        else:
            self.scaled_particles_data = self.particles_data
        self.train_test_split()
        self.lds_model = self.build_lds_model(em_iterations, T, d, M, L, True)
        self.reconstruction_results()
        self.write_particles(self.reconstructed_results)
    
    def build_lds_model(self, em_iterations, T=25, d=3, M=256, L=32, plot_likelihood=True):
        P = d * M
        initial_A = repeat(T-1, np.eye(L))
        initial_W = repeat(T, np.random.normal(0.0, 1.0, (P, L)))
        lds = LDS(n_dim_obs=P, n_dim_state=L,
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
            particles_tensor = torch.from_numpy(self.train_data).double().to(DEVICE)
            particles_mask = np.zeros(self.train_data.shape)
            lds = lds.em(particles_tensor, n_iter=1, observations_mask=particles_mask)
            end_em = timer()
            print(f'^^EM Done in {(end_em - start_em)/60} minutes, Computing Log-Likelihood ^^')
            start_log = timer()
            loglikelihoods[i] = lds.loglikelihood(particles_tensor, observations_mask=particles_mask)
            end_log = timer()
            print(f'Iteration {i}, log-likelihood = {loglikelihoods[i]} computed in {(end_log - start_log)/60} minutes \n\n')
        np.savetxt(f'{self.out_dir}/log_likelihood_{em_iterations}.txt', loglikelihoods)
        if plot_likelihood:
            plt.figure()
            plt.plot(loglikelihoods)
            plt.xlabel('EM Iteration')
            plt.ylabel('Average Log Likelihood')
            plt.savefig(f'{self.out_dir}/log_likelihood_{em_iterations}.png')
            print('EM Plot saved')
        print('===== LDS Analysis Model Built======')
        return lds

    def train_test_split(self):
        N = self.particles_data.shape[0]
        train_num = int(self.train_test_split_ratio * N)
        self.train_indices = random.sample(range(0, N), train_num)
        self.test_indices = list(set(range(0, N)) - set(self.train_indices))
        self.train_data = self.scaled_particles_data[self.train_indices, :, :]
        self.test_data = self.scaled_particles_data[self.test_indices, : , :]
    
    def reconstruction_results(self):
        test_particles_tensor = torch.from_numpy(self.test_data).double().to(DEVICE)
        test_particles_mask = np.zeros(self.test_data.shape)
        self.reconstructed_results = self.lds_model.reconstruct(observations=test_particles_tensor,
                                        observations_mask=test_particles_mask)
        print(f'Reconstruction done - reconstructed_results size = {self.reconstructed_results.size()}')

        mse_vals = torch.mean(((test_particles_tensor-self.reconstructed_results)**2), 2)
        mse_avg = torch.mean(mse_vals)
        print(f'MSE avg = {mse_avg} ')

    def write_particles(self, particles_tensor):
        particles_data = particles_tensor.to('cpu').numpy()
        N, T, P = particles_data.shape
        reconst_dir = f'{self.out_dir}/reconst/'
        if not os.path.exists(reconst_dir):
            os.makedirs(reconst_dir)
        for n in range(N):
            for t in range(T):
                fn = f'{reconst_dir}/{n}_{t}.particles'
                raw_data = particles_data[n, t]
                data_reshaped = raw_data.reshape((int(P//3), 3))
                np.savetxt(fn, data_reshaped)
        print('particles saved')