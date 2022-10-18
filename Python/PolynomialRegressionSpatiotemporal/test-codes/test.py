import shapeworks as sw
import numpy as np
from sklearn.model_selection import KFold
from sklearn.linear_model import Lasso, LassoCV, MultiTaskLassoCV
from RegressionUtils import *

MODELS_WORKING_DIR = '/home/sci/nawazish.khan/Public/Spatiotemporal-Polynomial-Regression-Experiments/'
# DATASET_NAME = 'Synthetic-Dataset'
DATASET_NAME = 'Left-Atrium-Dataset'
PROJECT_DIR = f'{MODELS_WORKING_DIR}/{DATASET_NAME}/'
PROJECT_FILE_NAME = f'{PROJECT_DIR}/spatiotemporal_regression_model.xlsx'

TIME_ARRAY = np.loadtxt(f'{PROJECT_DIR}/t_array.txt').astype(int)
assert TIME_ARRAY.shape[0] >= 1

# sanity check
shape_matrix = load_regression_inputs_from_particles(particles_dir=f'{PROJECT_DIR}/cross_sectional_model_particles/')
print(shape_matrix.shape)
estimate_parameters(shape_matrix, TIME_ARRAY, 1e-5)
