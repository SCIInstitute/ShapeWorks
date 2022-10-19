# import shapeworks as sw
import numpy as np
from sklearn.model_selection import KFold
from sklearn.linear_model import Lasso, LassoCV, MultiTaskLassoCV
# from RegressionUtils import *
import matplotlib.pyplot as plt

MODELS_WORKING_DIR = '/home/sci/nawazish.khan/Public/Spatiotemporal-Polynomial-Regression-Experiments/'
DATASET_NAME = 'Synthetic-Dataset'
PROJECT_DIR = f'{MODELS_WORKING_DIR}/{DATASET_NAME}/'
MODEL_NAME = 'spatiotemporal_regression_model'
PROJECT_FILE_NAME = f'{PROJECT_DIR}/{MODEL_NAME}.xlsx'
RELATIVE_ERROR_LOG_FILES = f'{PROJECT_DIR}/{MODEL_NAME}_REL_MSE_log'
SCORE_R2_LOG_FILES = f'{PROJECT_DIR}/{MODEL_NAME}_R2_SCORE_log'
FIXED_ALPHA_VAL = 1e-6

# Plot Rel MSE Errors and R2
plt.rcParams["figure.figsize"] = (20, 15)
fig, ax = plt.subplots()
with open(f'{RELATIVE_ERROR_LOG_FILES}.txt', 'r') as f:
    x = f.readlines()
    print(len(x))
errors = x[0].split('.')
errors_new = [f'0.{x}' for x in errors[1:]]
errors_new = np.asarray([float(x) for x in errors_new])
print(errors_new.shape)
rel_errors = errors_new
print(rel_errors.shape)
ax.plot(rel_errors)
textstr ='Relative Error at end of optimization = ' + '{0:.7f}'.format(rel_errors[-1])
props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
ax.text(0.05, 0.95, textstr, fontsize=18,transform=ax.transAxes,
        verticalalignment='top', bbox=props)
ax.set_ylabel('Relative Mean Squared Error')
ax.set_xlabel('Optimization Iterations')
plt.savefig(f'{RELATIVE_ERROR_LOG_FILES}.png', dpi=700)

#-------
plt.clf()
plt.rcParams["figure.figsize"] = (20, 15)
fig, ax = plt.subplots()
with open(f'{SCORE_R2_LOG_FILES}.txt', 'r') as f:
    x = f.readlines()
    print(len(x))
errors = x[0].split('.')
errors_new = [f'0.{x}' for x in errors[1:]]
errors_new = np.asarray([float(x) for x in errors_new])
print(errors_new.shape)
scores_r2 = errors_new
print(scores_r2.shape)
ax.plot(scores_r2)
textstr = 'Score at end of optimization = ' + '{0:.7f}'.format(scores_r2[-1])
props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
ax.text(0.05, 0.95, textstr, fontsize=18,transform=ax.transAxes,
        verticalalignment='top', bbox=props)
ax.set_ylabel('Coefficient of Determination')
ax.set_xlabel('Optimization Iterations')
plt.savefig(f'{SCORE_R2_LOG_FILES}.png', dpi=700)
