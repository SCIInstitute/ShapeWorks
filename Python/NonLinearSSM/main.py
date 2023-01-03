from model import InvertibleNetwork
from model import *

# Train Burn-In Model
particles_dir = ''

inv_net = InvertibleNetwork(init_particles_dir=particles_dir)
inv_net.train_invertible_network()


# Set up Shapeworks Optimizer Object
sw_opt = sw.Optimize()
def before_callback():
    inv_net.norm_model.eval()
    particles = sw_opt.GetParticleSystem()
    

    # Compute gradients

    # Set gradients
    
    pass
