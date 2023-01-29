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
M = params.num_particles
N = params.num_samples
d = params.dimension
burn_in_particles_dir = f'{WORKING_DIR}/{params.burn_in_dir}'
particles_dir = f'{WORKING_DIR}/{params.project_name}_particles'
project_file_path = f'{WORKING_DIR}/{params.project_name}.xlsx'

# Set up Shapeworks Optimizer Object
sw_opt = sw.Optimize()
# Create SW Project File with Optimization Params and burn_in_particles as landmarks
sw_opt.LoadXlsxProjectFile(project_file_path)

print('Project Loaded')
global inv_net
inv_net = InvertibleNetwork(params=params)
inv_net.initialize_model()
inv_net.initialize_particles(init_particles_dir=burn_in_particles_dir)
print('Burn in training....')
inv_net.train_invertible_network_from_scratch()

# Define Callbacks
def train_model_callback():
    if sw_opt.GetOptimizing():
        print('Train Model callback in Python 0....')
        # Make sure prior is updated properly before training for new iteration
        inv_net.initialize_model()
        inv_net.initialize_particles(init_particles_dir=particles_dir)
        inv_net.train_invertible_network_from_last_checkpoint()
        print('Train Model callback in Python 1....')

def update_base_particles_callback():
    if sw_opt.GetOptimizing():
        print(f'Updating Base Particles callback from Python 0....')
        inv_net.norm_model.eval()
        particles = sw_opt.GetParticleSystem() # Z space
        # print(f'Particle System shape {particles.shape}')
        particles = torch.from_numpy(particles.T).to(DEVICE)
        par_procc = particles.reshape(-1, M, 3)
        par_procc = par_procc[:,:,:d].reshape(N, -1)
        # print(f'Tensor shape {par_procc.size()}')
        z0_datas, lg, lls = inv_net.norm_model(par_procc.float())
        z0_data = z0_datas[-1]
        z0_particles = z0_data.detach().cpu().numpy()
        z0_particles = z0_particles.T
        sw_opt.SetNonLinearBaseShapeMatrix(z0_particles)
        print(f'Updating Base Particles callback from Python 1....')

def before_gradient_updates_callback():
    if sw_opt.GetOptimizing():
        print(f'Before Gradient updates callback from Python 0....')
        inv_net.norm_model.eval()
        particles = sw_opt.GetParticleSystem() # Z space
        # print(f'Particle System shape {particles.shape}')
        particles = torch.from_numpy(particles.T).to(DEVICE)
        par_procc = particles.reshape(-1, M, 3)
        par_procc = par_procc[:,:,:d].reshape(N, -1)
        # print(f'Tensor shape {par_procc.size()}')
        z0_datas, lg, lls = inv_net.norm_model(par_procc.float())
        z0_data = z0_datas[-1]
        z0_particles = z0_data.detach().cpu().numpy()
        z0_particles = z0_particles.T
        log_det_jacobian = lls.detach().cpu().numpy()
        # Size = number of samples N
        det_jacobian = np.exp(log_det_jacobian)
        print(f'det_jacobian size = {det_jacobian.shape}')
        det_jacobian = det_jacobian[:, None]
        det_jacobian = det_jacobian.T
        assert lls.size()[0] == N
        lls = lls.repeat(d*M, 1)

        # Difference matrix
        _, _, p_z_0 = inv_net.get_prior_distribution()
        sampled_pz = p_z_0.sample((N, ))
        # print(f'z0_particles shape = {z0_particles.shape} samples p_z size = {sampled_pz.size()} and final lls size = {lls.size()}')
        diff_matrix = sampled_pz.T * lls
        diff_matrix = diff_matrix.detach().cpu().numpy()
        # Set params for ShapeWorks Optimizer
        sw_opt.SetNonLinearBaseShapeMatrix(z0_particles)
        sw_opt.SetNonLinearJacobianMatrix(det_jacobian)
        sw_opt.SetNonLinearDifferenceMatrix(diff_matrix)
        sw_opt.ComputeBaseSpaceCovarianceMatrix()
        z0_cov = sw_opt.GetBaseSpaceInverseCovarianceMatrix() # dM X dM
        z0_mean = sw_opt.GetBaseSpaceMean() # dM vector
        # print(f'Mean shape is {z0_mean.shape}')
        inv_net.update_prior_distribution(z0_mean, z0_cov)
        print(f'Gradient updates callback from Python 1....')

# Set callbacks for SW Opt object
sw_opt.SetNonLinearTrainModelCallbackFunction(train_model_callback)
sw_opt.SetBeforeGradientUpdatesCallbackFunction(before_gradient_updates_callback)
sw_opt.SetUpdateBaseParticlesCallbackFunction(update_base_particles_callback)

# Run Optimizer
print("Running Shapeworks Optimization")
sw_opt.Run()
sw_opt.SaveProjectFileAfterOptimize()

