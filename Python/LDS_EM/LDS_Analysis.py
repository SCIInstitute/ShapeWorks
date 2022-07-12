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
    def __init__(self, model_name, em_iterations, T, d, M, L, scaling=True, train_test_split_ratio=0.8, baseline_model=False) -> None:
        self.model_name = model_name
        self.out_dir = f'{SHAPE_MODELS_DIR}/{self.model_name}_LDS_Analysis_L_{L}/'
        if not os.path.exists(self.out_dir):
            os.makedirs(self.out_dir)
        self.particles_data = load_correspondences(shapes_desc_file=f'{PROJECT_DIR}/description.json', 
                                                correspondences_dir=f'{SHAPE_MODELS_DIR}/{self.model_name}_particles/',
                                                T=T, d=d, M=M, baseline_model=baseline_model)
        self.train_test_split_ratio = train_test_split_ratio
        if scaling:
            self.scaling_obj = MinMaxScaler()
        else:
            self.scaling_obj = None
        if scaling:
            self.scaled_particles_data = self.scaling_obj.fit_transform(self.particles_data.reshape(-1, self.particles_data.shape[-1])).reshape(self.particles_data.shape)
        else:
            self.scaled_particles_data = self.particles_data
        self.train_test_split()
        self.lds_model = self.build_lds_model(em_iterations, T, d, M, L, True)
        self.reconstruction_results()
        self.write_particles(self.reconstructed_results)
        self.generalization_missing_time_points()
        self.specificity_results()
    
    def build_lds_model(self, em_iterations, T=25, d=3, M=256, L=32, plot_likelihood=True):
        P = d * M
        # initial_A = repeat(T-1, np.eye(L))
        print('using normal weightss')
        initial_A = repeat(T-1, np.random.laplace(0, 0.001, (L, L)))
        initial_W = repeat(T, np.random.laplace(0, 0.001, (P, L)))
        # initial_A = repeat(T-1, np.random.lognormal(0, 0.000001, (L, L)))
        # initial_W = repeat(T, np.random.lognormal(0, 0.000001, (P, L)))
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
        random.seed(SEED_VAL) # To ensure same train-test split between two models under comparison
        self.train_indices = random.sample(range(0, N), train_num)
        self.test_indices = list(set(range(0, N)) - set(self.train_indices))
        self.train_data = self.scaled_particles_data[self.train_indices, :, :]
        self.test_data = self.scaled_particles_data[self.test_indices, : , :]
    
    def specificity_results(self):
        NUM_SAMPLES = 12
        T = self.train_data.shape[1]
        dM = self.train_data.shape[2]
        spec_res = torch.zeros(NUM_SAMPLES, T, dM).to(DEVICE)
        for i in range(NUM_SAMPLES):
            _, obs_sample = self.lds_model.sample(NUM_TIME_POINTS)
            dist_closest_train_data = self.find_distance_with_closest_data(obs_sample) # T X dM
            spec_res[i] = dist_closest_train_data
        
        spec_res_mean = torch.sqrt(torch.mean(spec_res, -1)) # S X T
        # Save result
        res_np = spec_res_mean.to('cpu').numpy()
        res_fn = f'{self.out_dir}/specificity_{self.model_name}.txt'
        np.savetxt(res_fn, res_np)
        print(f'spec_res_mean size= {spec_res_mean.size()}')
        print(f'Mean Specificty = {torch.mean(spec_res_mean)} ')

    def find_distance_with_closest_data(self, sample):
        N = self.train_data.shape[0]
        T = self.train_data.shape[1]
        dM = self.train_data.shape[2]
        train_data_tensor = torch.from_numpy(self.train_data).double().to(DEVICE)
        distances_all_with_sample = torch.zeros(N).to(DEVICE)
        for i in range(N):
            sub = train_data_tensor[i] # T X dM
            distances_all_with_sample[i] = torch.mean((sub - sample)**2)
        closest_shape = torch.argmin(distances_all_with_sample)
        dist = (sample - train_data_tensor[closest_shape])**2
        return dist

    def generalization_missing_time_points(self):
        print('-----Computing Generalization for Missing Time Points ------')
        missing_time_points = [4, 8, 12, 16, 20]
        percents = []
        T = self.test_data.shape[1]
        N = self.test_data.shape[0]
        gen_errors = []
        for amount_missing in missing_time_points:
            percents.append(str(amount_missing*100/T)[:4]+'%')
            test_as_masked = np.ma.asarray(self.test_data)
            FIXED_SEED = 60
            # Mask Some Time Points for each subject
            for n_ix in range(self.test_data.shape[0]):
                random.seed(FIXED_SEED)
                FIXED_SEED += 1
                masked_indices = random.sample(range(0, T), amount_missing)
                test_as_masked[n_ix, masked_indices] = np.ma.masked
            test_mask = np.ma.getmask(test_as_masked)
            test_particles_tensor = torch.from_numpy(self.test_data).double().to(DEVICE)
            reconst_results_tensor = self.lds_model.reconstruct(observations=test_particles_tensor, observations_mask=test_mask)
            reconst_results_np = reconst_results_tensor.to('cpu').numpy()

            target_data = self.test_data[test_mask].reshape(N, int(np.sum(test_mask)/N))
            predicted_res = reconst_results_np[test_mask].reshape(N, int(np.sum(test_mask)/N))
            gen_error = np.sqrt(np.mean((target_data-predicted_res)**2, axis = -1)) # [1:]
            gen_errors.append(gen_error)
            print(f'------For {amount_missing} Missing Time Points, Avg Error = {np.mean(gen_error)}')
        
        # save results
        gen_errors = np.array(gen_errors)
        res_fn = f'{self.out_dir}/generalization_time_point_{self.model_name}.txt'
        np.savetxt(res_fn, gen_errors)
        print(f'gen_errors size= {gen_errors.shape}')
        print(f'Mean Error = {np.mean(gen_errors)} ')
        return None
    
    def reconstruction_results(self):
        test_particles_tensor = torch.from_numpy(self.test_data).double().to(DEVICE)
        test_particles_mask = np.zeros(self.test_data.shape)
        self.reconstructed_results = self.lds_model.reconstruct(observations=test_particles_tensor,
                                        observations_mask=test_particles_mask)
        # if self.scaling_obj is not None:
        #     reconst_np = self.reconstructed_results.to('cpu').numpy()
        #     reconstructed_results_np_scaled = self.scaling_obj.fit_transform(reconst_np.reshape(-1, reconst_np.shape[-1])).reshape(reconst_np.shape)
        #     self.reconstructed_results = torch.from_numpy(reconstructed_results_np_scaled).double().to(DEVICE)
        # print(f'Reconstruction done - reconstructed_results size = {self.reconstructed_results.size()}')

        gen_res = torch.sqrt(torch.mean(((test_particles_tensor-self.reconstructed_results)**2), 2))
        gen_avg = torch.mean(gen_res)
        print(f'Mean Generalization = {gen_avg} ')

        mse_np = gen_res.to('cpu').numpy()
        mse_fn = f'{self.out_dir}/generalization_{self.model_name}.txt'
        np.savetxt(mse_fn, mse_np)
        print(f'gen_res size = {gen_res.size()}')


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