from model import InvertibleNetwork
from model import *

# Shape Model Particles Directory
particles_dir = ''

global inv_net

inv_net = InvertibleNetwork(init_particles_dir=particles_dir)
inv_net.train_invertible_network()

def train_model_callback():
    inv_net = InvertibleNetwork(init_particles_dir=particles_dir)
    inv_net.train_invertible_network()
    pass

# Set up Shapeworks Optimizer Object
sw_opt = sw.Optimize()
def before_callback():
    inv_net.norm_model.eval()
    particles = sw_opt.GetParticleSystem()


    # Compute gradients

    # Set gradients
    
    pass
