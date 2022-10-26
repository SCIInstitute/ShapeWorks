import shapeworks as sw
import numpy as np
from sklearn.model_selection import KFold
from sklearn.linear_model import Lasso, LassoCV, MultiTaskLassoCV
from Estimate_1_cv import *
import matplotlib.pyplot as plt
plt.rcParams["figure.figsize"] = (20, 15)

MODELS_WORKING_DIR = '/home/sci/nawazish.khan/Public/Spatiotemporal-Polynomial-Regression-Experiments/'
DATASET_NAME = 'Left-Atrium-Dataset'
PROJECT_DIR = f'{MODELS_WORKING_DIR}/{DATASET_NAME}/'
ex = '-recent_1_500_opt'
MODEL_NAME = f'spatiotemporal_regression_model{ex}'
PROJECT_FILE_NAME = f'{PROJECT_DIR}/{MODEL_NAME}.xlsx'
RELATIVE_ERROR_LOG_FILES = f'{PROJECT_DIR}/{MODEL_NAME}_REL_MSE_log'
SCORE_R2_LOG_FILES = f'{PROJECT_DIR}/{MODEL_NAME}_R2_SCORE_log'
FIXED_ALPHA_VAL = 1e-6

opt = sw.Optimize()
opt.LoadXlsxProjectFile(PROJECT_FILE_NAME)
print(f'Project File loaded in optimizer object')
TIME_ARRAY = np.loadtxt(f'{PROJECT_DIR}/t_array.txt').astype(int)
assert TIME_ARRAY.shape[0] >= 1

def before_evaluate():
    print(f'Running Before Evaluate in python')
    print(f'------ Calling Residuals from python ------')
    residuals = opt.GetSpatiotemporalResiduals()
    print(f'------Got Residuals in python shape = {residuals.shape}-----')
    print(f'------ Calling Mean from python ------')
    current_mean = opt.GetSpatiotemporalMeanMatrix()
    print(f'------Got Mean in python shape = {current_mean.shape}-----')
    t_array = TIME_ARRAY
    X = residuals + current_mean
    new_mean = estimate_mean(X, t_array, fn_mse=f'{RELATIVE_ERROR_LOG_FILES}.txt', fn_r2=f'{SCORE_R2_LOG_FILES}.txt')
    opt.SetSpatiotemporalRegressionMeanMatrix(new_mean)

opt.SetBeforeEvaluateCallbackFunction(before_evaluate)
print('-----SetBeforeEvaluateCallbackFunction set from Python----')
print('-----Running Opt from Python----')
opt.Run()
opt.SaveProjectFileAfterOptimize(PROJECT_FILE_NAME)

# Plot Rel MSE Errors and R2
rel_errors = np.loadtxt(f'{RELATIVE_ERROR_LOG_FILES}.txt', dtype=float)
print(rel_errors.shape)
plt.plot(rel_errors)
plt.ylabel('Relative Mean Squared Error')
plt.xlabel('Optimization Iterations')
plt.savefig(f'{RELATIVE_ERROR_LOG_FILES}.png', dpi=700)

scores_r2 = np.loadtxt(f'{SCORE_R2_LOG_FILES}.txt', dtype=float)
print(scores_r2.shape)
plt.plot(scores_r2)
plt.ylabel('Coefficient of Determination')
plt.xlabel('Optimization Iterations')
plt.savefig(f'{SCORE_R2_LOG_FILES}.png', dpi=700)
