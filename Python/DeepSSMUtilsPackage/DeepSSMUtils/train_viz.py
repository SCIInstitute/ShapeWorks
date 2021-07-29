import os
import numpy as np

# Writes particles and error scalars for best, mean, and worst in the validation set
# predicted_particles and true_particles are numpy array with dims: # in validation set, # of particles, 3 coordinates
def write_validation_examples(predicted_particles, true_particles, out_dir):
	if not os.path.exists(out_dir):
		os.makedirs(out_dir)
	# get min, mean, and max errors
	mses = np.mean(np.mean((predicted_particles - true_particles)**2, axis=2), axis=1)
	mean_index = np.argmin(np.abs(mses - np.mean(mses)))
	indices = [np.argmin(mses), mean_index,  np.argmax(mses)]
	names = ["best", "mean", "worst"]
	for i in range(3):
		# get particles
		pred = predicted_particles[indices[i]]
		# write particle file
		out_particle_file = out_dir + "/" + names[i] + "_validation.particles"
		np.savetxt(out_particle_file, pred)
		# get scalar field for error
		out_scalar_file = out_dir + "/" + names[i] + "_validation.scalars"
		scalars = np.mean((pred - true_particles[indices[i]])**2, axis=1)
		np.savetxt(out_scalar_file, scalars)