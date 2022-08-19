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
        save_scaled_correspondences(self.scaling_obj, shapes_desc_file=f'{PROJECT_DIR}/description.json', 
                                                correspondences_dir=f'{SHAPE_MODELS_DIR}/{self.model_name}_particles/',
                                                T=T, d=d, M=M, baseline_model=baseline_model, return_desc=True)
        print(f'orig corres. saved...')
        self.save_reconst_scaled()    
        print(f'reconst corres. saved...')

    def save_reconst_scaled(self):
        # load reconst particles particles from
        reconst_dir = f'{self.out_dir}/gen_missing_time_point_particles_reconst/'
        print(reconst_dir)
        all_particle_files = glob.glob(f'{reconst_dir}/**/*.particles', recursive=True)
        print(len(all_particle_files))
        all_particles = []
        for idx, particle_file in enumerate(all_particle_files):
            all_particles.append(np.loadtxt(particle_file).flatten())
        all_particles = np.array(all_particles) # N X dM
        print(all_particles.shape)
        dM = all_particles.shape[1]
        all_particles_scaled = self.scaling_obj.fit_transform(all_particles).reshape(all_particles.shape)
        for idx, particle_file in enumerate(all_particle_files):
            dir_name = str(Path(particle_file).parent)
            p_name = str(Path(particle_file).stem)
            fn = f'{dir_name}/{p_name}_scaled.particles'
            particle_raw = all_particles_scaled[idx]
            data_reshaped = particle_raw.reshape((int(dM//3), 3))
            np.savetxt(fn, data_reshaped)
            print(f'{fn} saved .....')

        
    def apply_inverse_transform(self, np_arr):
        np_arr_unscaled = self.scaling_obj.inverse_transform(np_arr.reshape(-1, np_arr.shape[-1])).reshape(np_arr.shape)
        return np_arr_unscaled




l_val = 64
em_iters = 10
scale_val=True
model_names = ['model_A', 'model_B']



print(f'----Building model - {model_names[0]} for L = {l_val}, having {em_iters} EM iterations and scaling = {scale_val} ------')

analyze_k_fold_A = LDS_Analysis(model_name=model_names[0], em_iterations=em_iters, T=25, d=3, M=256, L=l_val, 
                                    scaling=scale_val, baseline_model=True, expt_type='K_FOLD')
# analyze_specificity_A = LDS_Analysis(model_name=model_names[0], em_iterations=em_iters, T=25, d=3, M=256, L=l_val, 
#                                     scaling=scale_val, baseline_model=True, expt_type='SPECIFICITY')



l_val = 64
em_iters = 10
scale_val=True
model_names = ['model_A', 'model_B']



print(f'----Building model - {model_names[1]} for L = {l_val}, having {em_iters} EM iterations and scaling = {scale_val} ------')

analyze_k_fold_B = LDS_Analysis(model_name=model_names[1], em_iterations=em_iters, T=25, d=3, M=256, L=l_val,
                                     scaling=scale_val, baseline_model=False, expt_type='K_FOLD')