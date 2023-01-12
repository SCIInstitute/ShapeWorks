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
        inv_net.initialize_model()
        inv_net.initialize_particles(init_particles_dir=particles_dir)
        inv_net.train_invertible_network_from_last_checkpoint()

def before_gradient_updates_callback():
    if sw_opt.GetOptimizing():
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
        sw_opt.ComputeZ0CovarianceMatrix()
        z0_cov = sw_opt.GetInverseCovarianceMatrix()
        z0_mean = sw_opt.GetMean()
        inv_net.update_prior_distribution(z0_mean, z0_cov)


# Set callbacks for SW Opt object
sw_opt.SetNonLinearTrainModelCallbackFunction(train_model_callback)
sw_opt.SetBeforeGradientUpdatesCallbackFunction(before_gradient_updates_callback)

# Run Optimizer
sw_opt.Run()
sw_opt.SaveProjectFileAfterOptimize()

