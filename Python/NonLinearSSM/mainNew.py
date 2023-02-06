from modelNew import InvertibleNetwork
import sys
from utils import DictMap
import json
import torch
import os

# Shape Model Particles Directory
param_fn = sys.argv[1]
print(f'Params loaded from file {param_fn}')
params = DictMap(json.load(open(param_fn)))

params.device = torch.device(params.gpu_device if torch.cuda.is_available() else 'cpu')
print(f'DEVICE = {params.device}')

WORKING_DIR = params.working_dir
MODEL_SAVE_DIR = f'{params.working_dir}/pytorch-models-new-1/'
params.output_dir = MODEL_SAVE_DIR
if not os.path.isdir(params.output_dir):
    os.makedirs(params.output_dir)
params.results_file = os.path.join(params.output_dir, params.results_file)

M = params.num_particles
N = params.num_samples
d = params.dimension
burn_in_particles_dir = f'{WORKING_DIR}/{params.burn_in_dir}'
particles_dir = f'{WORKING_DIR}/{params.project_name}_particles'
project_file_path = f'{WORKING_DIR}/{params.project_name}.xlsx'

global inv_net
inv_net = InvertibleNetwork(params=params)
inv_net.initialize_particles(init_particles_dir=burn_in_particles_dir, particle_system='warped')
inv_net.initialize_model()
print('Model Initialized, Now Training')
inv_net.train_model_from_scratch()
inv_net.serialize_model()

