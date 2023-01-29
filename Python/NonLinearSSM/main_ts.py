from random import sample
import shapeworks as sw
import argparse
from model import InvertibleNetwork
from model import *
import sys

# Shape Model Particles Directory
param_fn = sys.argv[1]
print(f'Params loaded from file {param_fn}')
params = DictMap(json.load(open(param_fn)))
DEVICE = torch.device(params.device if torch.cuda.is_available() else 'cpu')
print(f'DEVICE = {DEVICE}')
WORKING_DIR = params.working_dir
MODEL_SAVE_DIR = f'{params.working_dir}/pytorch-models/'
M = params.num_particles
N = params.num_samples
d = params.dimension
burn_in_particles_dir = f'{WORKING_DIR}/{params.burn_in_dir}'
particles_dir = f'{WORKING_DIR}/{params.project_name}_particles'
project_file_path = f'{WORKING_DIR}/{params.project_name}.xlsx'

global inv_net
inv_net = InvertibleNetwork(params=params)
inv_net.initialize_model()
inv_net.serialize_model()

