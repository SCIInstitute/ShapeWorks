import shapeworks as sw
import numpy as np
from hotelling.stats import hotelling_t2
import statsmodels.stats.multitest as multi
from shapeworks.utils import sw_message
from shapeworks.utils import sw_progress
from shapeworks.utils import sw_check_abort
from scipy import stats

'''
This function calculates the p-values per correspondence point for the group difference.
Input are:
data - pandas data frame with two columns file name and group ids
number_of_particles - number of correspondence particles 
permutations - number of times the pvalues are calculated
'''


def compute_pvalues_for_group_difference(data, number_of_particles, permutations=100):
    group_id = data["group_ids"].unique()
    group_0_filenames = list(data[data["group_ids"] == group_id[0]]["filename"])
    group_1_filenames = list(data[data["group_ids"] == group_id[1]]["filename"])

    group_0_data = sw.ParticleSystem(group_0_filenames).Particles()
    group_0_data = np.reshape(group_0_data, (number_of_particles, 3, -1))
    group_1_data = sw.ParticleSystem(group_1_filenames).Particles()
    group_1_data = np.reshape(group_1_data, (number_of_particles, 3, -1))

    return compute_pvalues_for_group_difference_data(group_0_data, group_1_data, permutations)


def compute_pvalues_for_group_difference_studio(group_0_data, group_1_data, permutations=100):
    number_of_particles = int(group_0_data.shape[0] / 3)
    group_0 = np.reshape(group_0_data, (number_of_particles, 3, -1))
    group_1 = np.reshape(group_1_data, (number_of_particles, 3, -1))
    return compute_pvalues_for_group_difference_data(group_0, group_1, permutations)


def compute_pvalues_for_group_difference_data(group_0_data, group_1_data, permutations=100):
    number_of_particles = group_0_data.shape[0]
    group_0_size = group_0_data.shape[-1]
    group_1_size = group_1_data.shape[-1]
    subset_size = min(group_0_size, group_1_size)

    pvalues_matrix = np.zeros((number_of_particles, permutations))
    idx = 0
    for p in range(permutations):
        sw_progress(float(idx) / permutations)
        idx = idx + 1
        if sw_check_abort():
            sw_message("Aborted")
            return

        group_0_index = np.random.choice(group_0_size, subset_size, replace=False)
        group_1_index = np.random.choice(group_1_size, subset_size, replace=False)

        group_0_subset = group_0_data[:, :, group_0_index]
        group_1_subset = group_1_data[:, :, group_1_index]

        for particle_id in range(number_of_particles):
            x = group_0_subset[particle_id, :, :].T
            y = group_1_subset[particle_id, :, :].T

            stats, f, pv, s = hotelling_t2(x, y)
            pvalues_matrix[particle_id, p] = pv
    corrected_pvalue_matrix = np.zeros((number_of_particles, 1))
    for particle_id in range(number_of_particles):
        r, pval = multi.fdrcorrection(pvalues_matrix[particle_id, :], alpha=0.05)
        corrected_pvalue_matrix[particle_id, 0] = np.mean(pval)
    return corrected_pvalue_matrix


def normalize(subj_map, group1_mean_map, group2_mean_map):
    slope = (2.0 / (group2_mean_map - group1_mean_map))
    subj_diff = subj_map - group1_mean_map
    subj_map_normalized = slope * subj_diff - 1
    return subj_map_normalized


def lda_loadings(group1_data, group2_data):
    group1_num = np.shape(group1_data)[1]
    group2_num = np.shape(group2_data)[1]

    combined_data = np.concatenate((group1_data, group2_data), axis=1)
    group1_mean = np.mean(group1_data, axis=1)
    group2_mean = np.mean(group2_data, axis=1)

    overall_mean = np.mean(combined_data, axis=1)

    diffVect = group1_mean - group2_mean

    group1_mean_diff = group1_mean - overall_mean
    group2_mean_diff = group2_mean - overall_mean

    group1_mean_map = np.dot(diffVect, group1_mean_diff)
    group2_mean_map = np.dot(diffVect, group2_mean_diff)

    group1_mean_map_normalized = normalize(group1_mean_map, group1_mean_map, group2_mean_map)
    group2_mean_map_normalized = normalize(group2_mean_map, group1_mean_map, group2_mean_map)

    group1_map = np.zeros((group1_num,))
    group2_map = np.zeros((group2_num,))

    for ii in range(group1_num):
        subjDiff = group1_data[:, ii] - overall_mean
        group1_map[ii] = np.dot(diffVect, subjDiff)
        group1_map[ii] = normalize(group1_map[ii], group1_mean_map, group2_mean_map)

    for ii in range(group2_num):
        subjDiff = group2_data[:, ii] - overall_mean
        group2_map[ii] = np.dot(diffVect, subjDiff)
        group2_map[ii] = normalize(group2_map[ii], group1_mean_map, group2_mean_map)

    group1_map_mean = group1_map.mean()
    group2_map_mean = group2_map.mean()

    group1_map_std = group1_map.std()
    group2_map_std = group2_map.std()

    group1_x = np.linspace(group1_map_mean - 6, group1_map_mean + 6, num=300)
    group2_x = np.linspace(group2_map_mean - 6, group2_map_mean + 6, num=300)

    group1_pdf = stats.norm.pdf(group1_x, group1_map_mean, group1_map_std)
    group2_pdf = stats.norm.pdf(group2_x, group2_map_mean, group2_map_std)
    return group1_x, group2_x, group1_pdf, group2_pdf, group1_map, group2_map


def lda(data):
    group_id = data["group_ids"].unique()
    group1_idxs = data.index[data['group_ids'] == 0].tolist()
    group2_idxs = data.index[data['group_ids'] == 1].tolist()
    filenames = data["filename"]
    # Provide the list of file names
    particle_data = sw.ParticleSystem(filenames)

    # Calculate the PCA for the read particle system
    shape_statistics = sw.ParticleShapeStatistics()
    shape_statistics.PCA(particleSystem=particle_data, domainsPerShape=1)

    # Calculate the loadings
    shape_statistics.principalComponentProjections()
    pca_loadings = shape_statistics.pcaLoadings().T
    group1_data = pca_loadings[:, group1_idxs]
    group2_data = pca_loadings[:, group2_idxs]

    return lda_loadings(group1_data, group2_data)
