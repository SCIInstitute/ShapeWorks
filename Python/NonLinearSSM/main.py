import shapeworks as sw
import argparse
from model import InvertibleNetwork
from model import *

# Shape Model Particles Directory
burn_in_particles_dir = '/home/sci/nawazish.khan/non-linear-ssm-experiments/data/supershapes_3_lobes_particles/'
particles_dir = '/home/sci/nawazish.khan/non-linear-ssm-experiments/data/supershapes_3_lobes_particles/'
project_file_path = '/home/sci/nawazish.khan/non-linear-ssm-experiments/data/supershapes_3_lobes.xlsx'

# Set up Shapeworks Optimizer Object
sw_opt = sw.Optimize()
# Create SW Project File with Optimization Params and burn_in_particles as landmarks
sw_opt.LoadXlsxProjectFile(project_file_path)

global inv_net
inv_net = InvertibleNetwork()
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
        particles = torch.from_numpy(particles.T).to(DEVICE)
        par_procc = particles.reshape(-1, M, 3)
        par_procc = par_procc[:,:,:d].reshape(N, -1)
        z0_datas, lg, lls = inv_net.norm_model(par_procc.float())
        z0_data = z0_datas[-1]
        z0_particles = z0_data.detach().cpu().numpy()
        sw_opt.SetNonLinearBaseShapeMatrix(z0_particles)
        print(f'Updating Base Particles callback from Python 1....')

def before_gradient_updates_callback():
    if sw_opt.GetOptimizing():
        print(f'Before Gradient updates callback from Python 0....')
        inv_net.norm_model.eval()
        particles = sw_opt.GetParticleSystem() # Z space
        particles = torch.from_numpy(particles.T).to(DEVICE)
        par_procc = particles.reshape(-1, M, 3)
        par_procc = par_procc[:,:,:d].reshape(N, -1)
        z0_datas, lg, lls = inv_net.norm_model(par_procc.float())
        z0_data = z0_datas[-1]
        z0_particles = z0_data.detach().cpu().numpy()
        log_det_jacobian = lls.detach().cpu().numpy()
        # Size = number of samples N
        det_jacobian = np.exp(log_det_jacobian)
        assert lls.size()[0] == N
        lls = lls.repeat(d*M, 1) 

        # Difference matrix
        _, _, p_z_0 = inv_net.get_prior_distribution()
        diff_matrix = p_z_0.sample((N, )) * lls
        diff_matrix = diff_matrix.detach().cpu().numpy()
        # Set params for ShapeWorks Optimizer
        sw_opt.SetNonLinearBaseShapeMatrix(z0_particles)
        sw_opt.SetNonLinearJacobianMatrix(det_jacobian)
        sw_opt.SetNonLinearDifferenceMatrix(diff_matrix)
        sw_opt.ComputeBaseSpaceCovarianceMatrix()
        z0_cov = sw_opt.GetBaseSpaceInverseCovarianceMatrix() # dM X dM
        z0_mean = sw_opt.GetBaseSpaceMean() # dM vector
        print(f'Mean shape is {z0_mean.shape}')
        inv_net.update_prior_distribution(z0_mean, z0_cov)
        print(f'Gradient updates callback from Python 1....')

# Set callbacks for SW Opt object
sw_opt.SetNonLinearTrainModelCallbackFunction(train_model_callback)
sw_opt.SetBeforeGradientUpdatesCallbackFunction(before_gradient_updates_callback)
sw_opt.SetUpdateBaseParticlesCallback(update_base_particles_callback)


# Run Optimizer
print("Running Shapeworks Optimization")
sw_opt.Run()
sw_opt.SaveProjectFileAfterOptimize()

