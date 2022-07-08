import os
import glob
from pathlib import Path
import json
import numpy as np
# import subprocess
# import shapeworks as sw
# import matplotlib.pyplot as plt
# import math

PRE_ABLATION = 'pre'
POST_ABLATION = 'post'
JOINT = ''
MODEL_TYPE = JOINT
NUM_TIME_POINTS = 25
DEVICE = 'cuda:1'

# Define dimensions
# N = 28 # number of samples
# d = 3 # dimension of correspondence points
# M = 256 # number of correspondence points
# L = 2 # dimension of latent space
# P = d*M  # dimension of observation space 
# T = 25   # number of time points

# Set up Paths here
PROJECT_DIR = '/home/sci/nawazish.khan/SSM-4D/'
SHAPE_MODELS_DIR = f'{PROJECT_DIR}shape_models/'


DEFAULT_OPT_PARAMS = {
        "number_of_particles" : 256,
        "use_normals": 0,
        "normals_strength": 3.0,
        "checkpointing_interval" : 1000,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 2500,
        "optimization_iterations" : 2500,
        "starting_regularization" : 1000.0,
        "ending_regularization" : 10.0,
        "recompute_regularization_interval" : 3,
        "domains_per_shape" : NUM_TIME_POINTS,
        "relative_weighting" : 1.0,
        "initial_relative_weighting" : 0.07,
        "procrustes" : 1,
        "procrustes_interval" : 1,
        "procrustes_scaling" : 1,
        "save_init_splits" : 0,
        "verbosity" : 2,
        "use_statistics_in_init" : 0,
        "adaptivity_mode": 0
}
# Utility functions
def make_dirs(dirs):
    for dir in dirs:
        if not os.path.exists(dir):
            os.makedirs(dir)

def sort_files_from_dir(files_dir, file_prefix='', mid_sub_string='', file_postfix='', ext='.vtk'):
    loaded_files_path = (glob.glob(f'{files_dir}/{file_prefix}*{mid_sub_string}*{file_postfix}{ext}'))
    sorted_files_path = sorted(loaded_files_path, key = lambda x : os.path.splitext(os.path.basename(x))[0])
    sorted_files_name = [Path(x).stem for x in sorted_files_path]
    return sorted_files_path, sorted_files_name

def build_shapes_info_json(meshes_dir):
    all_meshes, mesh_names = sort_files_from_dir(files_dir=meshes_dir)
    subj_set = set()
    for mesh_name in mesh_names:
        print(mesh_name)
        sub_name = mesh_name.split('_20')[0]
        subj_set.add(sub_name)
    shapes_info = {}
    shapes_info['ALL_SUBJECTS'] = list(subj_set)
    shapes_info_file = f'{meshes_dir}/description.json'
    for subject in subj_set:
        print(f'Sub -- {subject}')
        # get all pre shapes
        pre_shapes = glob.glob(f"{meshes_dir}/{subject}*_pre*")
        post_shapes = glob.glob(f"{meshes_dir}/{subject}*_post*")
        base_shapes = glob.glob(f"{meshes_dir}/{subject}*base*")
        shapes_info[subject] = {}
        shapes_info[subject]['PRE_ABLATION_TIME_POINTS'] = sorted(list(set(pre_shapes) - set(base_shapes)), key=lambda x: int(x.rsplit('.', 2)[-2]))
        shapes_info[subject]['POST_ABLATION_TIME_POINTS'] = sorted(list(set(post_shapes) - set(base_shapes)), key=lambda x: int(x.rsplit('.', 2)[-2]))
        shapes_info[subject]['BASE_SHAPES'] = sorted(base_shapes)
        print(f"len is {len(shapes_info[subject]['PRE_ABLATION_TIME_POINTS'])}")
    with open(shapes_info_file, "w") as outfile:
        json.dump(shapes_info, outfile)
    return shapes_info


def load_correspondences(shapes_desc_file, correspondences_dir, T=25, d=3, M=256):
    with open(shapes_desc_file) as json_file:
        shapes_desc_file = json.load(json_file)
    num_subjects = len(shapes_desc_file['ALL_SUBJECTS']) * 2 # PRE + POST for each patient
    corres_data = np.zeros((num_subjects, T, d*M))
    n = 0
    for subject in shapes_desc_file['ALL_SUBJECTS']:
        t = 0
        for shape_path in shapes_desc_file[subject]['PRE_ABLATION_TIME_POINTS']:
            sub_name = Path(shape_path).stem
            part_file_path = f'{correspondences_dir}/{sub_name}_local.particles'
            particles = np.loadtxt(part_file_path).flatten()
            corres_data[n][t] = particles
            t += 1
        assert t == T
        n += 1
        t = 0
        for shape_path in shapes_desc_file[subject]['PRE_ABLATION_TIME_POINTS']:
            sub_name = Path(shape_path).stem
            part_file_path = f'{correspondences_dir}/{sub_name}_local.particles'
            particles = np.loadtxt(part_file_path).flatten()
            corres_data[n][t] = particles
            t += 1
        assert t == T
        n += 1
    assert n == num_subjects
    print(f'Correspondence Particles loaded: shape = {corres_data.shape}')
    return corres_data


    



# def reconstruct_meshes_command(input_meshes_files, local_point_files, world_point_files, out_dir):
#         reconstructed_meshes_dir = f"{out_dir}/reconstructed_meshes/"
#         if not os.path.exists(reconstructed_meshes_dir):
#             os.makedirs(reconstructed_meshes_dir)
#         existing_reconst_meshes, _ = sort_files_from_dir(files_dir=reconstructed_meshes_dir)
#         median_shape_idx  = compute_median_shape(world_point_files=local_point_files)
#         if len(existing_reconst_meshes) != 0:
#             print(f'---------- Reconstructed Meshes Exist Already, Returning it ----------')
#             return existing_reconst_meshes, median_shape_idx
#         else:
#             print('---------- Reconstructed Mesh Files not found, Reconstructing now -----------')
#         execCommand = ["shapeworks", "warp-mesh", "--reference_mesh", input_meshes_files[median_shape_idx], 
#                     "--reference_points", local_point_files[median_shape_idx], "--save_dir", reconstructed_meshes_dir, "--target_points" ]
#         for fl in local_point_files:
#             execCommand.append(fl)
#         execCommand.append('--')
#         subprocess.check_call(execCommand)
#         print(f"---------- Mesh Reconstruction done at {reconstructed_meshes_dir} ----------")
#         reconstructed_mesh_files, _ = sort_files_from_dir(files_dir=reconstructed_meshes_dir)
#         if len(reconstructed_mesh_files) == 0:
#             raise ValueError('Mesh Reconstruction not done')
#         return reconstructed_mesh_files, 

    
# def compute_median_shape( world_point_files):
#         """
#             Utility function to find the median shape of the cohort, the one with Minimum L1 Norm
#         """
#         median_shape_idx = -1
#         min_sum = 1e10
#         shape_matrix = build_shape_matrix(world_point_files)
#         num_shapes = shape_matrix.shape[1]
#         for i in range(0, num_shapes):
#             cur_sum = 0.0
#             for j in range(0, num_shapes):
#                 if i != j:
#                     cur_sum += find_norm(shape_matrix, i, j)
#             if cur_sum < min_sum:
#                 min_sum = cur_sum
#                 median_shape_idx = i
#         if median_shape_idx == -1:
#             raise ValueError('Median shape not found for Reconstruction, Cannot proceed further with Mesh Warping')
#         return median_shape_idx


# def build_shape_matrix(world_point_files):
#         """
#             Utility function to build the shape matrix from the set of particles (world)
#         """
#         particle_sys = sw.ParticleSystem(world_point_files)
#         return particle_sys.Particles()



# def find_norm(self, shape_matrix, a, b):
#         """
#             Utility function to compute norm between two shape vectors 'a' and 'b' of the cohort
#         """
#         norm = 0.0
#         for i in range(0, shape_matrix.shape[0]):
#             norm += (math.fabs(shape_matrix[i, a] - shape_matrix[i, b]))
#         return norm

