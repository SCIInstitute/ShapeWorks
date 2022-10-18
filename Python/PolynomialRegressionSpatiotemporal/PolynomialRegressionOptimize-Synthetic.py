import shapeworks as sw
import numpy as np
from sklearn.model_selection import KFold
from sklearn.linear_model import Lasso, LassoCV, MultiTaskLassoCV
from RegressionUtils import *

MODELS_WORKING_DIR = '/home/sci/nawazish.khan/Public/Spatiotemporal-Polynomial-Regression-Experiments/'
DATASET_NAME = 'Synthetic-Dataset'
EXPT_NAME = 'spatiotemporal_regression_model.xlsx'
# DATASET_NAME = 'Left-Atrium-Dataset'
PROJECT_DIR = f'{MODELS_WORKING_DIR}/{DATASET_NAME}/'
PROJECT_FILE_NAME = f'{PROJECT_DIR}/{EXPT_NAME}'

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
    betas = estimate_parameters(X, t_array, 1e-5)
    opt.SetSpatiotemporalRegressionParameters(betas)

opt.SetBeforeEvaluateCallbackFunction(before_evaluate)
print('-----SetBeforeEvaluateCallbackFunction set from Python----')
print('-----Running Opt from Python----')
opt.Run()
opt.SaveProjectFileAfterOptimize(PROJECT_FILE_NAME)


