from shapeworks import ParticleSystem
import numpy as np
from hotelling.stats import hotelling_t2
import statsmodels.stats.multitest as multi



'''
This function calculates the p-values per correspondence point for the group difference.
Input are:
data - pandas data frame with two columns file name and group ids
number_of_particles - number of correspondence particles 
permutations - number of times the pvalues are calcualted
'''


def compute_pvalues_for_group_difference(data,number_of_particles,permutations = 100):
	
	group_id = data["group_ids"].unique()
	group_0_filenames = list(data[data["group_ids"]==group_id[0]]["filename"])
	group_1_filenames = list(data[data["group_ids"]==group_id[1]]["filename"])

	group_0_data = ParticleSystem(group_0_filenames).Particles()
	group_0_data = np.reshape(group_0_data,(number_of_particles,3,-1))
	group_1_data = ParticleSystem(group_1_filenames).Particles()
	group_1_data = np.reshape(group_1_data,(number_of_particles,3,-1))
	

	group_0_size = group_0_data.shape[-1]
	group_1_size = group_1_data.shape[-1]
	subset_size = min(group_0_size,group_1_size)

	
	pvalues_matrix = np.zeros((number_of_particles,permutations))
	for p in range(permutations):
		group_0_index = np.random.choice(group_0_size,subset_size,replace = False)
		group_1_index = np.random.choice(group_1_size,subset_size,replace = False)

		group_0_subset = group_0_data[:,:,group_0_index]
		group_1_subset = group_1_data[:,:,group_1_index]


		for particle_id in range(number_of_particles):

			
			x = group_0_subset[particle_id,:,:].T
			y = group_1_subset[particle_id,:,:].T
			
			stats,f,pv,s = hotelling_t2(x,y)
			pvalues_matrix[particle_id,p] = pv

	corrected_pvalue_matrix = np.zeros((number_of_particles,1))
	for particle_id in range(number_of_particles):
		r,pval = multi.fdrcorrection(pvalues_matrix[particle_id,:],alpha=0.05)
		corrected_pvalue_matrix[particle_id,0] = np.mean(pval)
	return corrected_pvalue_matrix

	