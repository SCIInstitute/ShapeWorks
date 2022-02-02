import os
import numpy as np

# Writes particles and error scalars for best, median, and worst 
# pred_particles and true_particles are numpy array with dims: # in set, # of particles, 3 coordinates
def write_examples(pred_particles, true_particles, out_dir):
	if not os.path.exists(out_dir):
		os.makedirs(out_dir)
	# get min, mean, and max errors
	mses = np.mean(np.mean((pred_particles - true_particles)**2, axis=2), axis=1)
	median_index = np.argsort(mses)[len(mses)//2]
	indices = [np.argmin(mses), median_index,  np.argmax(mses)]
	names = ["best", "median", "worst"]
	for i in range(3):
		# get particles
		pred = pred_particles[indices[i]]
		# write particle file
		out_particle_file = out_dir + names[i] + ".particles"
		np.savetxt(out_particle_file, pred)
		# get scalar field for error
		out_scalar_file = out_dir + names[i] + ".scalars"
		scalars = np.mean((pred - true_particles[indices[i]])**2, axis=1)
		np.savetxt(out_scalar_file, scalars)