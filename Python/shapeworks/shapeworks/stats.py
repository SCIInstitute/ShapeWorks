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


def compute_pvalues_for_group_difference(data, number_of_particles, permutations=100, seed=42):
    group_id = data["group_ids"].unique()
    group_0_filenames = list(data[data["group_ids"] == group_id[0]]["filename"])
    group_1_filenames = list(data[data["group_ids"] == group_id[1]]["filename"])

    group_0_data = sw.ParticleSystem(group_0_filenames).Particles()
    group_0_data = np.reshape(group_0_data, (number_of_particles, 3, -1))
    group_1_data = sw.ParticleSystem(group_1_filenames).Particles()
    group_1_data = np.reshape(group_1_data, (number_of_particles, 3, -1))

    return compute_pvalues_for_group_difference_data(group_0_data, group_1_data, permutations, seed)


def compute_pvalues_for_group_difference_studio(group_0_data, group_1_data, permutations=100, seed=42):
    number_of_particles = int(group_0_data.shape[0] / 3)
    group_0 = np.reshape(group_0_data, (number_of_particles, 3, -1))
    group_1 = np.reshape(group_1_data, (number_of_particles, 3, -1))
    return compute_pvalues_for_group_difference_data(group_0, group_1, permutations, seed)


def compute_pvalues_for_group_difference_data(group_0_data, group_1_data, permutations=100, seed=42):
    number_of_particles = group_0_data.shape[0]
    group_0_size = group_0_data.shape[-1]
    group_1_size = group_1_data.shape[-1]
    subset_size = min(group_0_size, group_1_size)

    rng = np.random.RandomState(seed)

    pvalues_matrix = np.zeros((number_of_particles, permutations))
    idx = 0
    for p in range(permutations):
        sw_progress(float(idx) / permutations)
        idx = idx + 1
        if sw_check_abort():
            sw_message("Aborted")
            return

        group_0_index = rng.choice(group_0_size, subset_size, replace=False)
        group_1_index = rng.choice(group_1_size, subset_size, replace=False)

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
    denom = group2_mean_map - group1_mean_map
    if abs(denom) < 1e-12:
        return 0.0
    slope = 2.0 / denom
    subj_diff = subj_map - group1_mean_map
    subj_map_normalized = slope * subj_diff - 1
    return subj_map_normalized


def lda_loadings(group1_data, group2_data):
    combined_data = np.concatenate((group1_data, group2_data), axis=1)
    group1_mean = np.mean(group1_data, axis=1)
    group2_mean = np.mean(group2_data, axis=1)

    diffVect = group1_mean - group2_mean

    return _project_and_pdf(diffVect, group1_data, group2_data, combined_data)


def _project_and_pdf(diffVect, group1_data, group2_data, combined_data, normalize_projections=True):
    """Shared logic for projecting groups onto a discriminant direction and fitting PDFs.

    Args:
        diffVect: Discriminant direction vector (features,)
        group1_data: PCA loadings for group 1 (features x samples)
        group2_data: PCA loadings for group 2 (features x samples)
        combined_data: Concatenation of group1_data and group2_data (features x all_samples)
        normalize_projections: If True, normalize so group means map to -1 and +1.
            This works well when diffVect is aligned with the mean difference (e.g. LDA).
            Set to False for directions that may not be aligned with the mean difference
            (e.g. DWD), which would cause the normalization to produce extreme values.

    Returns: 6-tuple (group1_x, group2_x, group1_pdf, group2_pdf, group1_map, group2_map)
    """
    group1_num = group1_data.shape[1]
    group2_num = group2_data.shape[1]

    group1_mean = np.mean(group1_data, axis=1)
    group2_mean = np.mean(group2_data, axis=1)
    overall_mean = np.mean(combined_data, axis=1)

    group1_mean_diff = group1_mean - overall_mean
    group2_mean_diff = group2_mean - overall_mean

    group1_mean_map = np.dot(diffVect, group1_mean_diff)
    group2_mean_map = np.dot(diffVect, group2_mean_diff)

    group1_map = np.zeros((group1_num,))
    group2_map = np.zeros((group2_num,))

    for ii in range(group1_num):
        subjDiff = group1_data[:, ii] - overall_mean
        group1_map[ii] = np.dot(diffVect, subjDiff)
        if normalize_projections:
            group1_map[ii] = normalize(group1_map[ii], group1_mean_map, group2_mean_map)

    for ii in range(group2_num):
        subjDiff = group2_data[:, ii] - overall_mean
        group2_map[ii] = np.dot(diffVect, subjDiff)
        if normalize_projections:
            group2_map[ii] = normalize(group2_map[ii], group1_mean_map, group2_mean_map)

    group1_map_mean = group1_map.mean()
    group2_map_mean = group2_map.mean()

    group1_map_std = group1_map.std()
    group2_map_std = group2_map.std()

    # Guard against zero std (all samples project to same point)
    min_std = 1e-6
    if group1_map_std < min_std:
        group1_map_std = min_std
    if group2_map_std < min_std:
        group2_map_std = min_std

    if normalize_projections:
        group1_x = np.linspace(group1_map_mean - 6, group1_map_mean + 6, num=300)
        group2_x = np.linspace(group2_map_mean - 6, group2_map_mean + 6, num=300)
    else:
        # Common x-range covering both groups and all shape mappings so PDF
        # tails extend smoothly across the full plot
        all_maps = np.concatenate([group1_map, group2_map])
        max_std = max(group1_map_std, group2_map_std)
        x_min = min(all_maps.min(), group1_map_mean - 6 * group1_map_std,
                    group2_map_mean - 6 * group2_map_std) - max_std
        x_max = max(all_maps.max(), group1_map_mean + 6 * group1_map_std,
                    group2_map_mean + 6 * group2_map_std) + max_std
        group1_x = np.linspace(x_min, x_max, num=300)
        group2_x = np.linspace(x_min, x_max, num=300)

    group1_pdf = stats.norm.pdf(group1_x, group1_map_mean, group1_map_std)
    group2_pdf = stats.norm.pdf(group2_x, group2_map_mean, group2_map_std)
    return group1_x, group2_x, group1_pdf, group2_pdf, group1_map, group2_map


def dwd_loadings(group1_data, group2_data):
    from dwd.gen_dwd import GenDWD
    group1_num = np.shape(group1_data)[1]
    group2_num = np.shape(group2_data)[1]

    if group1_num < 2 or group2_num < 2:
        raise ValueError(f"DWD requires at least 2 samples per group (got {group1_num} and {group2_num})")

    combined_data = np.concatenate((group1_data, group2_data), axis=1)

    # Fit GenDWD (samples x features)
    X = combined_data.T
    y = np.array([1]*group1_num + [-1]*group2_num)

    try:
        model = GenDWD(lambd=1.0)
        model.fit(X, y)
    except Exception as e:
        raise RuntimeError(f"DWD fitting failed: {e}") from e

    diffVect = model.coef_.flatten()

    # Normalize to unit length so projections reflect data geometry, not solver scale
    norm = np.linalg.norm(diffVect)
    if norm > 1e-12:
        diffVect = diffVect / norm

    # DWD's direction optimizes for margin, not mean separation, so it may be
    # nearly orthogonal to the mean difference. The mean-based normalization in
    # _project_and_pdf divides by the projection of the mean difference onto
    # diffVect, which can be near-zero, producing extreme values.
    # Use raw projections with adaptive PDF ranges instead.
    return _project_and_pdf(diffVect, group1_data, group2_data, combined_data, normalize_projections=False)


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
