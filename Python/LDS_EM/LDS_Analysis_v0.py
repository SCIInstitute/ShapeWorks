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
    # def __init__(self, model_name, em_iterations, T, d, M, L, scaling=True, train_test_split_ratio=0.8, baseline_model=False) -> None:
    #     self.model_name = model_name
    #     self.out_dir = f'{SHAPE_MODELS_DIR}/{self.model_name}_LDS_Analysis_L_{L}_scaling_{scaling}/'
    #     if not os.path.exists(self.out_dir):
    #         os.makedirs(self.out_dir)
    #     self.particles_data, self.subjects_array, self.patients_type = load_correspondences(shapes_desc_file=f'{PROJECT_DIR}/description.json', 
    #                                             correspondences_dir=f'{SHAPE_MODELS_DIR}/{self.model_name}_particles/',
    #                                             T=T, d=d, M=M, baseline_model=baseline_model, return_desc=True)
    #     self.train_test_split_ratio = train_test_split_ratio
    #     if scaling:
    #         self.scaling_obj = MinMaxScaler()
    #     else:
    #         self.scaling_obj = None
    #     if scaling:
    #         self.scaled_particles_data = self.scaling_obj.fit_transform(self.particles_data.reshape(-1, self.particles_data.shape[-1])).reshape(self.particles_data.shape)
    #     else:
    #         self.scaled_particles_data = self.particles_data
    #     self.train_test_split()
    #     self.lds_model = self.build_lds_model(em_iterations, T, d, M, L, True)
    #     self.reconstruction_results()
    #     self.write_particles(self.reconstructed_results)
    #     self.generalization_missing_time_points()
    #     self.specificity_results()

    def __init__(self, model_name, em_iterations, T, d, M, L, scaling=True, train_test_split_ratio=0.8, baseline_model=False, expt_type='K_FOLD') -> None:
        self.model_name = model_name
        self.out_dir = f'{SHAPE_MODELS_DIR}/{self.model_name}_LDS_Analysis_L_{L}_scaling_{scaling}/'
        if not os.path.exists(self.out_dir):
            os.makedirs(self.out_dir)
        self.particles_data, self.subjects_array, self.patients_type = load_correspondences(shapes_desc_file=f'{PROJECT_DIR}/description.json', 
                                                correspondences_dir=f'{SHAPE_MODELS_DIR}/{self.model_name}_particles/',
                                                T=T, d=d, M=M, baseline_model=baseline_model, return_desc=True)
        self.train_test_split_ratio = train_test_split_ratio
        if scaling:
            self.scaling_obj = MinMaxScaler()
        else:
            self.scaling_obj = None
        if scaling:
            self.scaled_particles_data = self.scaling_obj.fit_transform(self.particles_data.reshape(-1, self.particles_data.shape[-1])).reshape(self.particles_data.shape)
        else:
            self.scaled_particles_data = self.particles_data
        if expt_type == 'K_FOLD':
            self.k_fold_train_test_split()
            self.k_fold_lds_models = self.build_k_fold_lds_models(em_iterations, T, d, M, L, True)
            self.k_fold_reconstruction_results()
            self.k_fold_generalization_missing_time_points()
        elif expt_type == 'SPECIFICITY':
            self.train_data = self.scaled_particles_data
            print(f'-----Building LDS Model on 100 % Train Data for Specificity Experiment -------')
            self.lds_model = self.build_lds_model(em_iterations, T, d, M, L, True)
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


    def build_k_fold_lds_models(self, em_iterations, T=25, d=3, M=256, L=32, plot_likelihood=True):
        self.k_fold_lds_models = []
        K = 5
        for k_val in range(K):
            print(f'-----------Building k-fold LDS Models k = {k_val} ---------')
            P = d * M
            # initial_A = repeat(T-1, np.eye(L))
            initial_A = repeat(T-1, np.random.laplace(0, 0.01, (L, L)))
            initial_W = repeat(T, np.random.laplace(0, 0.01, (P, L)))
            # initial_A = repeat(T-1, np.random.laplace(0, 0.000001, (L, L)))
            # initial_W = repeat(T, np.random.laplace(0, 0.000001, (P, L)))
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
                particles_tensor = torch.from_numpy(self.k_fold_train_datas[k_val]).double().to(DEVICE)
                particles_mask = np.zeros(self.k_fold_train_datas[k_val].shape)
                lds = lds.em(particles_tensor, n_iter=1, observations_mask=particles_mask)
                end_em = timer()
                print(f'^^EM Done in {(end_em - start_em)/60} minutes, Computing Log-Likelihood ^^')
                start_log = timer()
                loglikelihoods[i] = lds.loglikelihood(particles_tensor, observations_mask=particles_mask)
                end_log = timer()
                print(f'Iteration {i}, log-likelihood = {loglikelihoods[i]} computed in {(end_log - start_log)/60} minutes \n\n')
            np.savetxt(f'{self.out_dir}/log_likelihood_K_Fold_{k_val}_{em_iterations}.txt', loglikelihoods)
            self.k_fold_lds_models.append(lds)
            if plot_likelihood:
                plt.figure()
                plt.plot(loglikelihoods)
                plt.xlabel('EM Iteration')
                plt.ylabel('Average Log Likelihood')
                plt.savefig(f'{self.out_dir}/log_likelihood_K_Fold_{k_val}_{em_iterations}.png')
                print('EM Plot saved')
            print(f'===== LDS Analysis Model {k_val} Built ======')
        assert len(self.k_fold_lds_models) == K
        return self.k_fold_lds_models

    def k_fold_train_test_split(self, K=5)->None:
        N = self.particles_data.shape[0]
        self.k_fold_train_datas = []
        self.k_fold_test_datas = []
        self.k_fold_train_indices = []
        self.k_fold_test_indices = []
        train_num = int(self.train_test_split_ratio * N)
        split_details_save_fn = f'{self.out_dir}/k_fold_details.txt'

        for k_val in range(K):
            random.seed(SEED_VAL+k_val) # To ensure same train-test split between two models under comparison
            train_indices_k = random.sample(range(0, N), train_num)
            test_indices_k = list(set(range(0, N)) - set(train_indices_k))
            self.k_fold_train_indices.append(train_indices_k)
            self.k_fold_test_indices.append(test_indices_k)
            self.k_fold_train_datas.append(self.scaled_particles_data[train_indices_k, :, :])
            self.k_fold_test_datas.append(self.scaled_particles_data[test_indices_k, : , :])
            with open(split_details_save_fn, "a") as f:
                f.write(f'K = {k_val} \n Train Indices : {train_indices_k} \n Subjects: \n')
                for idx in train_indices_k:
                    f.write(f' {self.subjects_array[idx]} - {self.patients_type[idx]} \n')
                f.write(f'K = {k_val} \n Test Indices : {test_indices_k} \n Subjects: \n')
                for idx in test_indices_k:
                    f.write(f' {self.subjects_array[idx]} - {self.patients_type[idx]} \n')
        print(f'{K} Fold train test split done')     
        return None

    def train_test_split(self):
        N = self.particles_data.shape[0]
        train_num = int(self.train_test_split_ratio * N)
        random.seed(SEED_VAL) # To ensure same train-test split between two models under comparison
        self.train_indices = random.sample(range(0, N), train_num)
        self.test_indices = list(set(range(0, N)) - set(self.train_indices))
        self.train_data = self.scaled_particles_data[self.train_indices, :, :]
        self.test_data = self.scaled_particles_data[self.test_indices, : , :]
    
    def specificity_results(self):
        print('-------Computing Specificity-------')
        # NUM_SAMPLES = self.train_data.shape[0] # do sampling S times = number of train shapes available or more
        NUM_SAMPLES = 100
        T = self.train_data.shape[1]
        dM = self.train_data.shape[2]
        spec_res = torch.zeros(NUM_SAMPLES, T, dM).to(DEVICE)
        sampled_tensors = torch.zeros(NUM_SAMPLES, T, dM).to(DEVICE)
        for i in range(NUM_SAMPLES):
            print(f'------Sampling | {i} ....')
            _, obs_sample = self.lds_model.sample(NUM_TIME_POINTS)
            sampled_tensors[i] = obs_sample
            dist_closest_train_data = self.find_distance_with_closest_data(obs_sample) # T X dM
            spec_res[i] = dist_closest_train_data
            # Save Sampled Result_sequence
            reconst_dir = f'{self.out_dir}/Specificity_Sampled_Particles/Sample_{i}/'
            if not os.path.exists(reconst_dir):
                os.makedirs(reconst_dir)
            obs_sample_np = obs_sample.to('cpu').numpy()
            obs_sample_np_unscaled = self.scaling_obj.inverse_transform(obs_sample_np)
            for t in range(T):
                fn = f'{reconst_dir}/Time_{t}.particles'
                particles_data = obs_sample_np_unscaled[t] # dM
                # Unscale to original limits
                data_reshaped = particles_data.reshape((int(dM//3), 3))
                np.savetxt(fn, data_reshaped)
                
        spec_res_mean = (torch.mean(spec_res, -1))/torch.mean(torch.square(sampled_tensors)) # S X T 
        # Save result
        res_np = spec_res_mean.to('cpu').numpy()
        res_fn = f'{self.out_dir}/Specificity_results_{self.model_name}.txt'
        np.savetxt(res_fn, res_np)
        # print(f'spec_res_mean size= {spec_res_mean.size()}')
        print(f'Mean Specificty = {torch.mean(spec_res_mean)} ')

    def relative_root_mean_sq_error(self, pred, x):
        # pred = N X T X dM
        # x = N X T X dM
        diff = pred-x
        error_val = torch.div(input=diff, other=pred) # N, T, dM
        rRMSE = torch.mean(torch.square(error_val), 2) # result --> N X T

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
            gen_error = (np.mean((target_data-predicted_res)**2, axis = -1)) # [1:] # remove sqrt
            gen_errors.append(gen_error)
            print(f'------For {amount_missing} Missing Time Points, Avg Error = {np.mean(gen_error)}')
        
        # save results
        gen_errors = np.array(gen_errors)
        res_fn = f'{self.out_dir}/generalization_time_point_{self.model_name}.txt'
        np.savetxt(res_fn, gen_errors)
        print(f'gen_errors size= {gen_errors.shape}')
        print(f'Mean Error = {np.mean(gen_errors)} ')
        return None

    def k_fold_generalization_missing_time_points_unscaled(self):
        print('-----Computing Generalization for Missing Time Points K Fold------')
        missing_time_points = [4, 8, 12, 16, 20]
        K = 5
        for k_val in range(K):
            test_data_k = self.k_fold_test_datas[k_val]
            T = test_data_k.shape[1]
            N = test_data_k.shape[0]
            gen_errors = []
            for amount_missing in missing_time_points:
                test_as_masked = np.ma.asarray(test_data_k)
                FIXED_SEED = 60 + k_val
                # Mask Some Time Points for each subject
                for n_ix in range(test_data_k.shape[0]):
                    random.seed(FIXED_SEED)
                    FIXED_SEED += 1
                    masked_indices = random.sample(range(0, T), amount_missing)
                    test_as_masked[n_ix, masked_indices] = np.ma.masked
                test_mask = np.ma.getmask(test_as_masked)
                test_particles_tensor = torch.from_numpy(test_data_k).double().to(DEVICE)
                k_fold_reconstructed_results = self.k_fold_lds_models[k_val].reconstruct(observations=test_particles_tensor, observations_mask=test_mask)
                reconst_results_np = k_fold_reconstructed_results.to('cpu').numpy()

                target_data = test_data_k[test_mask].reshape(N, int(np.sum(test_mask)/N))
                predicted_res = reconst_results_np[test_mask].reshape(N, int(np.sum(test_mask)/N))
                target_data_unscaled = self.scaling_obj.inverse_transform(target_data)
                predicted_res_unscaled = self.scaling_obj.inverse_transform(predicted_res)
                gen_error = (np.mean((target_data_unscaled-predicted_res_unscaled)**2, axis = -1))#[1:] # remove sqrt
                gen_errors.append(gen_error)
                print(f'------For {amount_missing} Missing Time Points, Avg Error = {np.mean(gen_error)}')
                # Save Particles
                # Save Reconstructed_results
                # particles_data = k_fold_reconstructed_results.to('cpu').numpy()
                # # Unscale to original limits
                # particles_data_unscaled = self.apply_inverse_transform(particles_data)
                # assert particles_data.shape == particles_data_unscaled.shape
                # N_test, T, P = particles_data_unscaled.shape
                # reconst_dir = f'{self.out_dir}/gen_missing_time_point_correspondence_particles_reconstructed_k_fold/K_{k_val}/'
                # if not os.path.exists(reconst_dir):
                #     os.makedirs(reconst_dir)
                # assert N_test == len(self.k_fold_test_indices[k_val])
                # for idx, n in enumerate(self.k_fold_test_indices[k_val]):
                #     for t in range(T):
                #         part_fn = f'{self.subjects_array[n]}_{self.patients_type[n]}_{t}.particles'
                #         fn = f'{reconst_dir}/{part_fn}'
                #         raw_data = particles_data_unscaled[idx, t]
                #         data_reshaped = raw_data.reshape((int(P//3), 3))
                #         np.savetxt(fn, data_reshaped)


            # save results
            gen_errors = np.array(gen_errors)
            res_fn = f'{self.out_dir}/generalization_time_point_{self.model_name}_K_Fold_{k_val}.txt'
            np.savetxt(res_fn, gen_errors)
            # print(f'gen_errors size= {gen_errors.shape}')
            print(f'Mean Generalization for K = {k_val} = {np.mean(gen_errors)} ')
    
    def k_fold_generalization_missing_time_points(self):
        print('-----Computing Generalization for Missing Time Points K Fold------')
        missing_time_points = [4, 8, 12, 16, 20]
        K = 5
        S_T = self.k_fold_test_datas[0].shape[0]
        MP_val = len(missing_time_points)
        gen_errors_all = np.zeros((K * MP_val, S_T))
        for k_val in range(K):
            test_data_k = self.k_fold_test_datas[k_val]
            T = test_data_k.shape[1]
            N = test_data_k.shape[0]
            gen_errors_k = []
            for amt_idx, amount_missing in enumerate(missing_time_points):
                test_as_masked = np.ma.asarray(test_data_k)
                FIXED_SEED = 60 + k_val
                # Mask Some Time Points for each subject
                # Keep track of missing time indices to mark particles while saving
                missing_time_instances_track = [] # of len S_T number of test subjects
                for n_ix in range(test_data_k.shape[0]):
                    random.seed(FIXED_SEED)
                    FIXED_SEED += 1
                    masked_indices = random.sample(range(0, T), amount_missing)
                    test_as_masked[n_ix, masked_indices] = np.ma.masked
                    missing_time_instances_track.append(masked_indices)
                test_mask = np.ma.getmask(test_as_masked)
                test_particles_tensor = torch.from_numpy(test_data_k).double().to(DEVICE)
                k_fold_reconstructed_results = self.k_fold_lds_models[k_val].reconstruct(observations=test_particles_tensor, observations_mask=test_mask)
                reconst_results_np = k_fold_reconstructed_results.to('cpu').numpy()

                target_data = test_data_k[test_mask].reshape(N, int(np.sum(test_mask)/N))
                predicted_res = reconst_results_np[test_mask].reshape(N, int(np.sum(test_mask)/N))
                # gen_error = (np.mean((target_data-predicted_res)**2, axis = -1)) # MSE Metric
                gen_error = (np.mean((target_data-predicted_res)**2, axis = -1))/np.mean(np.square(predicted_res)) # MSE Metric # result is num_test, dim

                gen_errors_k.append(gen_error)
                start_idx =  k_val * MP_val + amt_idx
                gen_errors_all[start_idx, :] = (gen_error)
                print(f'-----For K={k_val} and {amount_missing} Missing Time Points, error = {np.mean(gen_error)}')
                # Save Reconstructed Particles
                particles_data = k_fold_reconstructed_results.to('cpu').numpy()
                # Unscale to original limits
                particles_data_unscaled = self.apply_inverse_transform(particles_data)
                assert particles_data.shape == particles_data_unscaled.shape
                N_test, T, P = particles_data_unscaled.shape
                reconst_dir = f'{self.out_dir}/gen_missing_time_point_particles_reconst/K_{k_val}/TP_MISSING_{amount_missing}/'
                if not os.path.exists(reconst_dir):
                    os.makedirs(reconst_dir)
                assert N_test == len(self.k_fold_test_indices[k_val])
                for idx, n in enumerate(self.k_fold_test_indices[k_val]):
                    missing_time_instances_for_n = missing_time_instances_track[idx]
                    for t in range(T):
                        reconst_type = ''
                        if t in missing_time_instances_for_n:
                            reconst_type = 'MISSING'
                        else:
                            reconst_type = 'REGULAR'
                        part_fn = f'{self.subjects_array[n]}_{self.patients_type[n]}_{t}_{reconst_type}.particles'
                        fn = f'{reconst_dir}/{part_fn}'
                        raw_data = particles_data_unscaled[idx, t]
                        data_reshaped = raw_data.reshape((int(P//3), 3))
                        np.savetxt(fn, data_reshaped)
            print(f'------For K = {k_val}, Avg Error = {np.mean(np.array(gen_errors_k))}')
        # save results
        # gen_errors_all = np.array(gen_errors_all)
        res_fn = f'{self.out_dir}/generalization_missing_time_point_results_K_FOLD{self.model_name}.txt'
        np.savetxt(res_fn, gen_errors_all)

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

        # gen_res = (torch.mean(((test_particles_tensor-self.reconstructed_results)**2), 2)) # remove sqrt
        # gen_avg = torch.mean(gen_res)
        # print(f'Mean Generalization = {gen_avg} ')

        # mse_np = gen_res.to('cpu').numpy()
        # mse_fn = f'{self.out_dir}/generalization_{self.model_name}.txt'
        # np.savetxt(mse_fn, mse_np)
        # print(f'gen_res size = {gen_res.size()}')

        reconst_np = self.reconstructed_results.to('cpu').numpy()
        test_unscaled = self.apply_inverse_transform(self.test_data)
        reconst_np_unscaled = self.apply_inverse_transform(reconst_np)
        gen_res = (np.mean(((test_unscaled-reconst_np_unscaled)**2), 2)) # remove sqrt
        gen_avg = np.mean(gen_res)
        print(f'Mean Generalization = {gen_avg} ')

        mse_np = gen_res
        mse_fn = f'{self.out_dir}/generalization_{self.model_name}.txt'
        np.savetxt(mse_fn, mse_np)
        print(f'gen_res size = {gen_res.shape}')


    def k_fold_reconstruction_results_unscaled(self):
        print('-------Running K Fold Generalization Test -------')
        K = 5
        k_fold_reconstructed_results = [None] * K

        for k_val in range(K):
            test_data_k = self.k_fold_test_datas[k_val]
            test_particles_tensor = torch.from_numpy(test_data_k).double().to(DEVICE)
            test_particles_mask = np.zeros(test_data_k.shape)
            k_fold_reconstructed_results[k_val] = self.k_fold_lds_models[k_val].reconstruct(observations=test_particles_tensor, 
                                                                                observations_mask=test_particles_mask)
            if self.scaling_obj is None:
                # not scaling
                gen_res = (torch.mean(((test_particles_tensor-k_fold_reconstructed_results[k_val])**2), 2)) # remove sqrt
                gen_avg = torch.mean(gen_res)
                print(f'Mean Generalization for K = {k_val} = {gen_avg} ')
                mse_np = gen_res.to('cpu').numpy()
            else:
                reconst_particles_np = k_fold_reconstructed_results[k_val].to('cpu').numpy()
                reconst_particles_unscaled = self.apply_inverse_transform(reconst_particles_np)
                test_data_k_unscaled = self.apply_inverse_transform(test_data_k)
                # reconst_particles_unscaled = (reconst_particles_np)
                # test_data_k_unscaled = (test_data_k)
                gen_res = (np.mean(((test_data_k_unscaled-reconst_particles_unscaled)**2), 2)) # remove sqrt
                # gen_res = torch.sqrt(torch.mean(((test_data_k-reconst_particles)**2), 2))/torch.norm(reconst_particles) # remove sqrt
                gen_avg = np.mean(gen_res)
                print(f'Mean Generalization for K = {k_val} = {gen_avg} ')
                mse_np = gen_res
            mse_fn = f'{self.out_dir}/generalization_{self.model_name}_K_Fold_{k_val}.txt'
            np.savetxt(mse_fn, mse_np)
            # print(f'gen_res size = {gen_res.shape}')
            print(f'---------Results reconstructed for K = {k_val} -------------')

            # Save Reconstructed_results
            particles_data = k_fold_reconstructed_results[k_val].to('cpu').numpy()
            # Unscale to original limits
            # particles_data_unscaled = self.apply_inverse_transform(particles_data)
            particles_data_unscaled = (particles_data)
            assert particles_data.shape == particles_data_unscaled.shape
            N_test, T, P = particles_data_unscaled.shape
            reconst_dir = f'{self.out_dir}/gen_correspondence_particles_reconstructed_k_fold/K_{k_val}/'
            if not os.path.exists(reconst_dir):
                os.makedirs(reconst_dir)
            assert N_test == len(self.k_fold_test_indices[k_val])
            for idx, n in enumerate(self.k_fold_test_indices[k_val]):
                for t in range(T):
                    part_fn = f'{self.subjects_array[n]}_{self.patients_type[n]}_{t}.particles'
                    fn = f'{reconst_dir}/{part_fn}'
                    raw_data = particles_data_unscaled[idx, t]
                    data_reshaped = raw_data.reshape((int(P//3), 3))
                    np.savetxt(fn, data_reshaped)
                    # print(f'Particles saved {part_fn}')
    
    def k_fold_reconstruction_results(self):
        print('-------Running K Fold Generalization Test with RRMSE-------')
        K = 5
        k_fold_reconstructed_results = [None] * K
        generalization_particles_error_ar = []
        S_T = len(self.k_fold_test_indices[0])
        T = self.k_fold_test_datas[0].shape[1]
        dM = self.k_fold_test_datas[0].shape[2]
        d = 3
        M = int(dM//3)
        generalization_results_ar = np.zeros((K*S_T, T))
        generalization_particles_error_ar = np.zeros((K, M))
        for k_val in range(K):
            test_data_k = self.k_fold_test_datas[k_val]
            test_particles_tensor = torch.from_numpy(test_data_k).double().to(DEVICE)
            test_particles_mask = np.zeros(test_data_k.shape)
            k_fold_reconstructed_results[k_val] = self.k_fold_lds_models[k_val].reconstruct(observations=test_particles_tensor, 
                                                                                observations_mask=test_particles_mask)
            # gen_res = (torch.sqrt(torch.mean(((test_particles_tensor-k_fold_reconstructed_results[k_val])**2), 2)))/torch.norm(k_fold_reconstructed_results[k_val]) # RRMSE Metric
            gen_res = ((torch.mean(((test_particles_tensor-k_fold_reconstructed_results[k_val])**2), 2)))/torch.mean(torch.square(k_fold_reconstructed_results[k_val])) # RMSE Metric
            # done in the usual for each x,y,z dimension; result is N_Test_Sub X NUM_TIME_POINTS
            gen_avg = torch.mean(gen_res)
            start_idx = k_val * S_T
            generalization_results_ar[start_idx:start_idx+S_T, :] = (gen_res.to('cpu').numpy()) # S_T X T
            particle_error_xyz = (torch.mean(torch.mean(((test_particles_tensor-k_fold_reconstructed_results[k_val])**2), 1), 0))/torch.mean(torch.square(k_fold_reconstructed_results[k_val])) #dM
            particle_error_per_particle = torch.mean(torch.reshape(particle_error_xyz, (d, M)), 0) # reshape and take mean per particle --> M dim result
            # 1 - across time points, 0- across test subjects
            generalization_particles_error_ar[k_val] = (particle_error_per_particle.to('cpu').numpy())
            print(f'Mean Generalization for K = {k_val} = {gen_avg} ')
            print(f'---------Results reconstructed for K = {k_val} -------------')

        # generalization_results_ar = np.array(generalization_results_ar)
        # generalization_particles_error_ar = np.array(generalization_particles_error_ar)
        gen_result_fn = f'{self.out_dir}/generalization_results_{self.model_name}_K_Fold.txt'
        gen_particle_error_fn = f'{self.out_dir}/generalization_particle_results_{self.model_name}_K_Fold.txt'
        np.savetxt(gen_result_fn, generalization_results_ar)
        np.savetxt(gen_particle_error_fn, generalization_particles_error_ar)
        print('Results saved')
    
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

    def apply_inverse_transform(self, np_arr):
        np_arr_unscaled = self.scaling_obj.inverse_transform(np_arr.reshape(-1, np_arr.shape[-1])).reshape(np_arr.shape)
        return np_arr_unscaled