from Constants import *
import numpy as np
import math
import seaborn as sns
import pandas as pd
from scipy.stats import pearsonr
import matplotlib.pyplot as plt
import json
from pathlib import Path
from timeit import default_timer as timer


import LDS_model
import utils

class LDS_Analysis:
    def __init__(self, model_name, em_iterations, T, d, M, L) -> None:
        self.model_name = model_name
        self.out_dir = f'{SHAPE_MODELS_DIR}/{self.model_name}_LDS_Analysis_L_new_1{L}/'
        if os.path.exists(self.out_dir):
            os.makedirs(self.out_dir)
        self.particles_data = load_correspondences(shapes_desc_file=f'{PROJECT_DIR}/description.json', 
                                                correspondences_dir=f'{SHAPE_MODELS_DIR}/{self.model_name}_particles/',
                                                T=T, d=d, M=M)
        self.lds_model = self.build_lds_model(em_iterations, T, d, M, L, True)
    
    def build_lds_model(self, em_iterations, T, d, M, L, plot_likelihood=True):
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
            lds = lds.em(self.particles_data, n_iter=1)
            end_em = timer()
            print(f'^^EM Done in {(end_em - start_em)/60} minutes, Computing Log-Likelihood ^^')
            start_log = timer()
            loglikelihoods[i] = lds.loglikelihood(self.particles_data)
            end_log = timer()
            print(f'Iteration {i}, log-likelihood = {loglikelihoods[i]} computed in {(end_log - start_log)/60} minutes \n\n')
        np.savtext(f'{self.out_dir}/log_likelihood_{em_iterations}.txt', loglikelihoods)
        if plot_likelihood:
            plt.figure()
            plt.plot(loglikelihoods)
            plt.xlabel('EM Iteration')
            plt.ylabel('Average Log Likelihood')
            plt.savefig(f'{self.out_dir}/log_likelihood_{em_iterations}.png')
        return lds

print('new model - 1')
analyze_cross_entropy_model = LDS_Analysis(model_name='pre_post_model_2500',
                                             em_iterations=100, T=25, d=3, M=256, L=16)
