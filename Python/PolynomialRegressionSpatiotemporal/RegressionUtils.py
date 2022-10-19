import numpy as np
from sklearn.model_selection import KFold
from sklearn.linear_model import Lasso, LassoCV, MultiTaskLassoCV
import glob
import os
from pathlib import Path
import shapeworks as sw
import json

MODELS_WORKING_DIR = '/home/sci/nawazish.khan/Public/Spatiotemporal-Polynomial-Regression-Experiments/'
ALPHA_VALUES = [1e-5, 1e-4, 1e-3,1e-2,1e-1, 1, 10]
OPT_PARAMS = {}
with open('optimization_params.json', 'r') as f:
    OPT_PARAMS = json.load(f)

def load_regression_inputs_from_particles(particles_dir):
    particles_list_temp, file_names = sort_files_from_dir(files_dir=particles_dir,ext='_world.particles')
    N = len(particles_list_temp)
    assert N >= 1
    M = np.loadtxt(particles_list_temp[0]).shape[0]
    shape_matrix = np.zeros(((3*M), N))
    for idx, particle_file in enumerate(particles_list_temp):
        ar = np.loadtxt(particle_file).flatten()
        shape_matrix[:, idx] = ar
    return shape_matrix

def estimate_parameters(shape_matrix, t, alpha_value, fn_mse='temp_mse.txt', fn_r2 = 'temp_r2.txt'):
    print(f'----------ESTIMATING BETAS----------')
    N = t.shape[0]
    assert N == shape_matrix.shape[1]
    TOTAL_TIME_PTS = int(np.max(t))
    degree = TOTAL_TIME_PTS - 1
    # degree = 1
    dM = shape_matrix.shape[0]
    X = np.zeros((N, degree))

    for deg in range(0, degree):
        X[:, deg] = np.power(t, deg+1)
    
    shape_matrix = np.transpose(shape_matrix)
    mean_shape = np.tile(np.mean(shape_matrix, axis=0), (N,1))
    # print(f'New shape _matrix shape {shape_matrix.shape} and X shape = {X.shape}')

    # lassoreg = Lasso(alpha=alpha_value, normalize=True, max_iter=100000)
    lassoreg = Lasso(alpha=alpha_value)
    # lassoreg = MultiTaskLassoCV(alphas=ALPHA_VALUES)
    lassoreg.fit(X, shape_matrix)
    # print(f'-----Lasso Fit done alpha chosen {lassoreg.alpha_}')
    z_hat = lassoreg.predict(X)
    # print(f'z_hat shape {z_hat.shape}')
    mse = np.mean((z_hat - shape_matrix)**2)
    rel_mse = mse/np.mean((mean_shape - shape_matrix)**2)
    print(f'################ Relative MSE = {rel_mse} #################')
    with open(fn_mse, 'a') as f:
        f.write(f'str({rel_mse})\n')
    score_r2 = lassoreg.score(X, shape_matrix)
    with open(fn_r2, 'a') as f:
        f.write(f'str({score_r2})\n')
    print(f'################ R2 Score = {score_r2} #################')
    betas = np.zeros((dM, degree+1))
    betas[:,0]= lassoreg.intercept_
    betas[:,1:] = lassoreg.coef_
    print(f'----------BETAS ESTIMATED SUCCESSFULLY--------')
    return betas

def sort_files_from_dir(files_dir, file_prefix='', mid_sub_string='', file_postfix='', ext='.vtk', exclude_keyword=None):
    loaded_files_path = (glob.glob(f'{files_dir}/{file_prefix}*{mid_sub_string}*{file_postfix}{ext}'))
    sorted_files_path = sorted(loaded_files_path, key = lambda x : os.path.splitext(os.path.basename(x))[0])
    sorted_files_name = [Path(x).stem for x in sorted_files_path]
    if exclude_keyword is not None:
        for fp in sorted_files_path:
            if exclude_keyword in fp:
                sorted_files_path.remove(fp)
        for fn in sorted_files_name:
            if exclude_keyword in fn:
                sorted_files_name.remove(fn)
    return sorted_files_path, sorted_files_name

def get_time_index(file_name, dataset_name = ''):
    if dataset_name == 'Synthetic-Dataset':
        split_str = '_TIME'
        time_idx = file_name.split(split_str)[1]
        return str(int(time_idx))
    else:
        split_str = '_pre.'
        time_idx = file_name.split(split_str)[1].split('_')[0]
        return str(int(time_idx)+1)
    
class ShapeWorksProjectFile:
    def __init__(self, dataset_name) -> None:
        self.dataset_name = dataset_name
        self.meshes_dir = f'{MODELS_WORKING_DIR}/{dataset_name}/meshes/'
        self.project_out_dir = f'{MODELS_WORKING_DIR}/{dataset_name}/'
        self.opt_params = OPT_PARAMS[dataset_name]
    def create_cross_sectional_project_file(self, use_relative_paths=True, expt_name=''):
        optimization_params = self.opt_params['cross_sectional']
        file_paths, _ = sort_files_from_dir(files_dir=self.meshes_dir, ext='.vtk')
        file_names = []
        for fp in file_paths:
            fn = fp.split('/')[-1].split('.vtk')[0]
            file_names.append(fn)

        time_indices_ar = []
        for file_name in file_names:
            time_indices_ar.append(get_time_index(file_name, self.dataset_name))
        assert len(time_indices_ar) == len(file_paths)
        project_subjects = []
        project_location = self.project_out_dir
        for idx in range(len(file_paths)):
            subject = sw.Subject()
            subject.set_number_of_domains(1)
            if use_relative_paths:
                rel_mesh_files = sw.utils.get_relative_paths([file_paths[idx]], project_location)
            else:
                rel_mesh_files = [file_paths[idx]]
            subject.set_original_filenames(rel_mesh_files)
            subject.set_groomed_filenames(rel_mesh_files)
            project_subjects.append(subject)
        

        shapeworks_project = sw.Project()
        parameters = sw.Parameters()
        for key in optimization_params:
            parameters.set(key, sw.Variant([optimization_params[key]]))
        parameters.set("domain_type", sw.Variant('mesh'))
        shapeworks_project.set_subjects(project_subjects)
        shapeworks_project.set_parameters("optimize", parameters)
        project_name = f'{self.project_out_dir}/cross_sectional_model{expt_name}.xlsx'
        shapeworks_project.save(project_name)
        # print(f' Project Created at {project_name}')
    
    def create_spatiotemporal_regression_project_file(self, use_relative_paths=True, expt_name='', use_corresponding_csm=False):

        optimization_params = self.opt_params['spatiotemporal_regression']
        file_paths, _ = sort_files_from_dir(files_dir=self.meshes_dir, ext='.vtk')
        file_names = []
        for fp in file_paths:
            fn = fp.split('/')[-1].split('.vtk')[0]
            file_names.append(fn)

        time_indices_ar = []
        for file_name in file_names:
            time_indices_ar.append(get_time_index(file_name, self.dataset_name))
        assert len(time_indices_ar) == len(file_paths)
        # print(len(file_paths))


        project_subjects = []
        project_location = self.project_out_dir
        for idx in range(len(file_paths)):
            subject = sw.Subject()
            subject.set_number_of_domains(1)
            particles_dir_csm = 'cross_sectional_model_particles{expt_name}' if use_corresponding_csm else 'cross_sectional_model_particles'
            landmark_file = glob.glob(f'{self.project_out_dir}/{particles_dir_csm}/*{file_names[idx]}_local.particles')
            # print(landmark_file)
            # print(len(landmark_file))
            assert len(landmark_file) == 1
            if use_relative_paths:
                rel_mesh_files = sw.utils.get_relative_paths([file_paths[idx]], project_location)
                rel_landmark_files = sw.utils.get_relative_paths([landmark_file[0]], project_location)
            else:
                rel_mesh_files = [file_paths[idx]]
                rel_landmark_files = [landmark_file[0]]
            subject.set_original_filenames(rel_mesh_files)
            subject.set_groomed_filenames(rel_mesh_files)
            subject.set_landmarks_filenames(rel_landmark_files)
            # print(f'time index = {[time_indices_ar[idx]]}')
            subject.set_time_point_for_subject([time_indices_ar[idx]])
            project_subjects.append(subject)
        

        shapeworks_project = sw.Project()
        parameters = sw.Parameters()
        for key in optimization_params:
            parameters.set(key, sw.Variant([optimization_params[key]]))
        parameters.set("domain_type", sw.Variant('mesh'))
        shapeworks_project.set_subjects(project_subjects)
        shapeworks_project.set_parameters("optimize", parameters)
        project_name = f'{self.project_out_dir}/spatiotemporal_regression_model{expt_name}.xlsx'
        shapeworks_project.save(project_name)
        t_file  = f'{self.project_out_dir}/t_array.txt'
        time_indices_ar_int = [int(x) for x in time_indices_ar]
        np.savetxt(t_file, np.asarray(time_indices_ar_int).astype(int), fmt='%i')
        # print(f' Project Created at {project_name}')
    def estimate_initial_alpha_value(self):
        pass

def create_project_files(dataset, use_relative_paths=True, expt_name=''):
    ob = ShapeWorksProjectFile(dataset)
    # ob.create_cross_sectional_project_file(use_relative_paths, expt_name='')
    ob.create_spatiotemporal_regression_project_file(use_relative_paths, expt_name, False)

def estimate_alpha_values(dataset=''):
    ob = ShapeWorksProjectFile(dataset)
    ob.estimate_initial_alpha_value()
