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
K = 5
EVALUATION_METRIC = RMSE
NORMALIZED = False
print(f'K = {K} and Evaluation Metric set to {EVALUATION_METRIC} - Normalization {NORMALIZED}....... ')
class LDS_Analysis:
    def __init__(self, model_name, em_iterations, T, d, M, L, scaling=True, train_test_split_ratio=0.8, baseline_model=False, expt_type='K_FOLD') -> None:
        self.model_name = model_name
        self.out_dir = f'{SHAPE_MODELS_DIR}/{self.model_name}_LDS_Analysis_L_{L}_scaling_{scaling}_{EVALUATION_METRIC}/'
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
        # initial_A = repeat(T-1, np.random.normal(0, 0.001, (L, L)))
        # initial_W = repeat(T, np.random.normal(0, 0.001, (P, L)))
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
        # K = 5
        for k_val in range(K):
            print(f'-----------Building k-fold LDS Models k = {k_val} ---------')
            P = d * M
            # initial_A = repeat(T-1, np.eye(L))
            initial_A = repeat(T-1, np.random.laplace(0, 0.001, (L, L)))
            initial_W = repeat(T, np.random.laplace(0, 0.001, (P, L)))
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

    def k_fold_train_test_split(self)->None:
        N = self.particles_data.shape[0]
        self.k_fold_train_datas = []
        self.k_fold_test_datas = []
        self.k_fold_train_indices = []
        self.k_fold_test_indices = []
        train_num = int(self.train_test_split_ratio * N)
        split_details_save_fn = f'{self.out_dir}/k_fold_details.txt'

        for k_val in range(K):
            random.seed(SEED_VAL+k_val) # To ensure same train-test split between the two models under comparison
            train_indices_k = random.sample(range(0, N), train_num)
            test_indices_k = list(set(range(0, N)) - set(train_indices_k))
            self.k_fold_train_indices.append(train_indices_k)
            self.k_fold_test_indices.append(test_indices_k)
            self.k_fold_train_datas.append(self.scaled_particles_data[train_indices_k, :, :])
            self.k_fold_test_datas.append(self.scaled_particles_data[test_indices_k, : , :])
            with open(split_details_save_fn, "w") as f:
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
        print(f'-------Computing Specificity using {EVALUATION_METRIC}-------')
        NUM_SAMPLES = 500
        T = self.train_data.shape[1]
        dM = self.train_data.shape[2]
        spec_res = torch.zeros(NUM_SAMPLES, T, dM).to(DEVICE)
        sampled_tensors = torch.zeros(NUM_SAMPLES, T, dM).to(DEVICE)
        closest_train_datas = torch.zeros(NUM_SAMPLES, T, dM).to(DEVICE)
        for i in range(NUM_SAMPLES):
            print(f'------Sampling | {i} ....')
            _, obs_sample = self.lds_model.sample(NUM_TIME_POINTS)
            sampled_tensors[i] = obs_sample
            dist_closest_train_data, closest_train_sub = self.find_distance_with_closest_data(obs_sample) # T X dM
            
            #v1 - save distance to compute mean later
            # spec_res[i] = dist_closest_train_data
            #v2 
            closest_train_datas[i] = closest_train_sub # save closest data and compute rMSE in new manner
            
            # Save Sampled Result_sequence
            reconst_dir = f'{self.out_dir}/Specificity_Sampled_Particles/Sample_{i}/'
            if not os.path.exists(reconst_dir):
                os.makedirs(reconst_dir)
            obs_sample_np = obs_sample.to('cpu').numpy()
            if self.scaling_obj is not None:
                obs_sample_np_unscaled = self.scaling_obj.inverse_transform(obs_sample_np)
            else:
                obs_sample_np_unscaled = obs_sample_np
            for t in range(T):
                fn = f'{reconst_dir}/Time_{t}.particles'
                particles_data = obs_sample_np_unscaled[t] # dM
                # Unscale to original limits
                data_reshaped = particles_data.reshape((int(dM//3), 3))
                np.savetxt(fn, data_reshaped)

        #v1 - compute mean from saved distances all together
        # spec_res_mean = (torch.mean(spec_res, -1))/torch.mean(torch.square(sampled_tensors)) # S X T 
        #v2 
        spec_res_mean = self.evaluate_error(pred=sampled_tensors, x=closest_train_datas,reduction_type=ON_PARTICLE) # S X T 

        # Save result
        res_np = spec_res_mean.to('cpu').numpy()
        res_fn = f'{self.out_dir}/Specificity_results_{self.model_name}.txt'
        np.savetxt(res_fn, res_np)
        # print(f'spec_res_mean size= {spec_res_mean.size()}')
        print_result(f'Mean Specificty = {torch.mean(spec_res_mean)} ')
        print_result(f'STD Specificty = {torch.std(spec_res_mean)} ')


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
        return dist, train_data_tensor[closest_shape]

    def k_fold_generalization_missing_time_points(self):
        print(f'-----Computing Generalization for Missing Time Points K Fold using {EVALUATION_METRIC}------')
        missing_time_points = [4, 8, 12, 16, 20]
        # K = 5
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

                target_data = test_data_k[test_mask].reshape(N, int(np.sum(test_mask)/N)) # num_test X all_unmasked_particles_unrolled
                predicted_res = reconst_results_np[test_mask].reshape(N, int(np.sum(test_mask)/N))

                #v1 MSE Metric
                # gen_error = (np.mean((target_data-predicted_res)**2, axis = -1)) # MSE Metric
                #v2 rMSE
                # gen_error = (np.mean((target_data-predicted_res)**2, axis = -1))/np.mean(np.square(predicted_res)) #result is num_test
                #v3
                gen_error = self.evaluate_error(pred=predicted_res, x=target_data, reduction_type=ON_LAST_AXIS, numpy_array_dtype=True) #result is num_test in dim
                
                gen_errors_k.append(gen_error)
                start_idx =  k_val * MP_val + amt_idx
                gen_errors_all[start_idx, :] = (gen_error)
                print(f'-----For K={k_val} and {amount_missing} Missing Time Points, error = {np.mean(gen_error)}')
                # Save Reconstructed Particles
                particles_data = k_fold_reconstructed_results.to('cpu').numpy()
                # Unscale to original limits
                if self.scaling_obj is not None:
                    particles_data_unscaled = self.apply_inverse_transform(particles_data)
                else:
                    particles_data_unscaled = particles_data
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
        res_fn = f'{self.out_dir}/generalization_missing_time_point_results_K_FOLD_{self.model_name}.txt'
        np.savetxt(res_fn, gen_errors_all)
        print_result(f'{self.model_name} GENERALIZATION MISSING TIMEPOINTS STD = {np.std(gen_errors_all)}')
        print_result(f'{self.model_name} GENERALIZATION MISSING TIMEPOINTS = {np.mean(gen_errors_all)}')


    
    def k_fold_reconstruction_results(self):
        print(f'-------Computing K Fold Generalization Resluts using {EVALUATION_METRIC}-------')
        # K = 5
        k_fold_reconstructed_results = [None] * K
        generalization_particles_error_ar = []
        S_T = len(self.k_fold_test_indices[0])
        T = self.k_fold_test_datas[0].shape[1]
        dM = self.k_fold_test_datas[0].shape[2]
        d = 3
        M = int(dM//3)
        generalization_results_ar = np.zeros((K*S_T, T))
        generalization_particles_error_ar = np.zeros((K, M))
        generalization_particles_error_ar_all_TP = torch.zeros(K, T,  dM).to(DEVICE)
        
        for k_val in range(K):
            test_data_k = self.k_fold_test_datas[k_val]
            test_particles_tensor = torch.from_numpy(test_data_k).double().to(DEVICE)
            test_particles_mask = np.zeros(test_data_k.shape)
            k_fold_reconstructed_results[k_val] = self.k_fold_lds_models[k_val].reconstruct(observations=test_particles_tensor, 
                                                                                observations_mask=test_particles_mask)
            #v1
            # gen_res = (torch.sqrt(torch.mean(((test_particles_tensor-k_fold_reconstructed_results[k_val])**2), 2)))/torch.norm(k_fold_reconstructed_results[k_val]) # rRMSE Metric - Initial version
            #v2
            # gen_res = ((torch.mean(((test_particles_tensor-k_fold_reconstructed_results[k_val])**2), 2)))/torch.mean(torch.square(k_fold_reconstructed_results[k_val])) # rMSE Metric
            #v3
            gen_res = self.evaluate_error(pred=k_fold_reconstructed_results[k_val], x=test_particles_tensor, reduction_type=ON_PARTICLE)
            gen_avg = torch.mean(gen_res)
            start_idx = k_val * S_T
            generalization_results_ar[start_idx:start_idx+S_T, :] = (gen_res.to('cpu').numpy()) # S_T X T

            # # Initial Approach - v1
            # particle_error_xyz = (torch.mean(torch.mean(((test_particles_tensor-k_fold_reconstructed_results[k_val])**2), 1), 0))/torch.mean(torch.square(k_fold_reconstructed_results[k_val])) #dM
            # particle_error_per_particle = torch.mean(torch.reshape(particle_error_xyz, (d, M)), 0) # reshape and take mean per particle --> M dim result
            # # 1 - across time points, 0- across test subjects

            # New Approach - v2
            particle_error_xyz = self.evaluate_error(pred=k_fold_reconstructed_results[k_val], x=test_particles_tensor, reduction_type=ON_SUB_AND_TIME) #dM
            # this is for k fold_error
            particle_error_per_particle = torch.mean(torch.reshape(particle_error_xyz, (d, M)), 0) # reshape and take mean per particle --> M dim result
            generalization_particles_error_ar_all_TP[k_val] = self.evaluate_error(pred=k_fold_reconstructed_results[k_val], x=test_particles_tensor, reduction_type=ON_SUBJECT) # T X dM
      
            # 1 - across time points, 0- across test subjects
            generalization_particles_error_ar[k_val] = (particle_error_per_particle.to('cpu').numpy())
            print(f'Mean Generalization for K = {k_val} = {gen_avg} ')
            print(f'---------Results reconstructed for K = {k_val} -------------')

        # generalization_results_ar = np.array(generalization_results_ar)
        # generalization_particles_error_ar = np.array(generalization_particles_error_ar)
        # generalization_particles_error_ar_all_TP_mean_across_K_folds = torch.mean(generalization_particles_error_ar_all_TP, 0) # T X dM
        self.save_per_particle_error_stats(generalization_particles_error_ar_all_TP)

        gen_result_fn = f'{self.out_dir}/generalization_results_{self.model_name}_K_Fold.txt'
        gen_particle_error_fn = f'{self.out_dir}/generalization_particle_results_{self.model_name}_K_Fold.txt'
        np.savetxt(gen_result_fn, generalization_results_ar)
        np.savetxt(gen_particle_error_fn, generalization_particles_error_ar)
        print('Results saved')
        print_result(f'{self.model_name} GENERALIZATION STD = {np.std(generalization_particles_error_ar)}')
        print_result(f'{self.model_name} GENERALIZATION = {np.mean(generalization_particles_error_ar)}')
    
    def save_per_particle_error_stats(self, gen_k_folds_all, d=3, M=256):
        gen_mean_all = torch.mean(gen_k_folds_all, 0) # T X dM
        gen_plus_sigma = gen_mean_all + 1.5 * torch.std(gen_k_folds_all, 0)
        gen_minus_sigma = gen_mean_all - 1.5 * torch.std(gen_k_folds_all, 0)


        error_dir = f'{self.out_dir}/particle_wise_error_files/'
        if not os.path.exists(error_dir):
            os.makedirs(error_dir)
        for t in range(gen_mean_all.shape[0]):
            # T time points
            fn_mean = f'{error_dir}/Time_{t}_Error_mean.txt'
            fn_plus = f'{error_dir}/Time_{t}_Error_plus_sigma.txt'
            fn_minus = f'{error_dir}/Time_{t}_Error_minus_sigma.txt'

            res = torch.mean(torch.reshape(gen_mean_all[t], (d, M)), 0)
            np.savetxt(fn_mean, res.to('cpu').numpy())
            res_1 = torch.mean(torch.reshape(gen_plus_sigma[t], (d, M)), 0)
            np.savetxt(fn_plus, res_1.to('cpu').numpy())
            res_2 = torch.mean(torch.reshape(gen_minus_sigma[t], (d, M)), 0)
            np.savetxt(fn_minus, res_2.to('cpu').numpy())
        return None

    def apply_inverse_transform(self, np_arr):
        np_arr_unscaled = self.scaling_obj.inverse_transform(np_arr.reshape(-1, np_arr.shape[-1])).reshape(np_arr.shape)
        return np_arr_unscaled

    def root_mean_sq_error(self, pred, x, reduction_type=ON_PARTICLE, numpy_array_dtype=False, relative=False):
        # pred = N X T X dM
        # x = N X T X dM
        diff = pred-x
        if not numpy_array_dtype:
            x_bar = torch.max(x) - torch.min(x) 
        else:
            x_bar = np.amax(x) - np.amin(x)
        # divide element wise by predicted values
        if relative:
            if not numpy_array_dtype:
                error_val = torch.div(input=diff, other=pred) # N, T, dM
            else:
                error_val = np.divide(diff, pred) # N, T, dM
        else:
            error_val = diff

        #reduce now
        if reduction_type==ON_PARTICLE:
            # used for generalization, specificity
            if not numpy_array_dtype:
                rRMSE = torch.sqrt(torch.mean(torch.square(error_val), 2)) # result --> N X T
            else:
                rRMSE = np.sqrt(np.mean(torch.square(error_val), 2)) # result --> N X T
            result = rRMSE if not NORMALIZED else (rRMSE/x_bar)
        if reduction_type==ON_SUBJECT:
            if not numpy_array_dtype:
                rRMSE = (torch.mean(torch.square(error_val), 0)) # result --> T X dM
            else:
                rRMSE = (np.mean(torch.square(error_val), 0)) # result --> TX dM
            result = rRMSE if not NORMALIZED else (rRMSE/x_bar)
        elif reduction_type==ON_SUB_AND_TIME:
            # used for missing time point particle wise error differences
            if not numpy_array_dtype:
                rRMSE_per_particle = torch.sqrt(torch.mean(torch.mean(torch.square(error_val), 1), 0)) # result --> dM
            else:
                rRMSE_per_particle = np.sqrt(np.mean(np.mean(np.square(error_val), 1), 0)) # result --> dM
            result = rRMSE_per_particle if not NORMALIZED else (rRMSE_per_particle/x_bar)
        elif reduction_type==ON_LAST_AXIS:
            # used for missing time point generalization error values
            if not numpy_array_dtype:
                rRMSE_per_sub = torch.sqrt(torch.mean(torch.square(error_val), -1)) # result --> is dim[0]
            else:
                rRMSE_per_sub = np.sqrt(np.mean(np.square(error_val), -1)) # result --> is dim[0]
            result = rRMSE_per_sub if not NORMALIZED else (rRMSE_per_sub/x_bar)
        return result

    
    def mean_sq_error(self, pred, x, reduction_type=ON_PARTICLE, numpy_array_dtype=False, relative=False):
        # pred = N X T X dM
        # x = N X T X dM
        diff = pred-x
        if not numpy_array_dtype:
            x_bar = torch.max(x) - torch.min(x) 
        else:
            x_bar = np.amax(x) - np.amin(x)
        if relative:
            if not numpy_array_dtype:
                error_val = torch.div(input=diff, other=pred) # N, T, dM
            else:
                error_val = np.divide(diff, pred) # N, T, dM
        else:
            error_val = diff
        #reduce now
        if reduction_type==ON_PARTICLE:
            if not numpy_array_dtype:
                rMSE = (torch.mean(torch.square(error_val), 2)) # result --> N X T
            else:
                rMSE = (np.mean(torch.square(error_val), 2)) # result --> N X T
            result = rMSE if not NORMALIZED else (rMSE/x_bar)
        if reduction_type==ON_SUBJECT:
            if not numpy_array_dtype:
                rMSE = (torch.mean(torch.square(error_val), 0)) # result --> T X dM
            else:
                rMSE = (np.mean(torch.square(error_val), 0)) # result --> TX dM
            result = rMSE if not NORMALIZED else (rMSE/x_bar)
        elif reduction_type==ON_SUB_AND_TIME:
            if not numpy_array_dtype:
                rMSE_per_particle = (torch.mean(torch.mean(torch.square(error_val), 1), 0)) # result --> dM
            else:
                rMSE_per_particle = (np.mean(np.mean(np.square(error_val), 1), 0)) # result --> dM
            result = rMSE_per_particle if not NORMALIZED else (rMSE_per_particle/x_bar) 
        elif reduction_type==ON_LAST_AXIS:
            if not numpy_array_dtype:
                rMSE_per_sub = (torch.mean(torch.square(error_val), -1)) # result --> is dim[0]
            else:
                rMSE_per_sub = (np.mean(np.square(error_val), -1)) # result --> is dim[0]
            result = rMSE_per_sub if not NORMALIZED else (rMSE_per_sub/x_bar)
        return result


    def evaluate_error(self, pred, x, reduction_type=ON_PARTICLE, numpy_array_dtype=False):
        if EVALUATION_METRIC == RMSE_REL:
            return self.root_mean_sq_error(pred, x, reduction_type, numpy_array_dtype, relative=True)
        elif EVALUATION_METRIC == MSE_REL:
            return self.mean_sq_error(pred, x, reduction_type, numpy_array_dtype, relative=True)
        elif EVALUATION_METRIC == MSE:
            return self.mean_sq_error(pred, x, reduction_type, numpy_array_dtype)
        elif EVALUATION_METRIC == RMSE:
            return self.root_mean_sq_error(pred, x, reduction_type, numpy_array_dtype)
