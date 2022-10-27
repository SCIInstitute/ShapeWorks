import shapeworks as sw
import numpy as np
import sklearn.decomposition
from sklearn.linear_model import MultiTaskLassoCV, Lasso, MultiTaskElasticNetCV, LinearRegression, MultiTaskElasticNet, ElasticNet, ElasticNetCV
import numpy as np
from sklearn.linear_model import Lasso, LassoCV, MultiTaskLassoCV
import matplotlib.pyplot as plt
plt.rcParams["figure.figsize"] = (20, 15)

MODELS_WORKING_DIR = '/home/sci/nawazish.khan/Public/Spatiotemporal-Polynomial-Regression-Experiments/'
DATASET_NAME = 'Left-Atrium-Dataset'
PROJECT_DIR = f'{MODELS_WORKING_DIR}/{DATASET_NAME}/'
ex = '-linear_model_final_expt'
MODEL_NAME = f'spatiotemporal_regression_model{ex}'
PROJECT_FILE_NAME = f'{PROJECT_DIR}/{MODEL_NAME}.xlsx'
RELATIVE_ERROR_LOG_FILES = f'{PROJECT_DIR}/{MODEL_NAME}_REL_MSE_log'
SCORE_R2_LOG_FILES = f'{PROJECT_DIR}/{MODEL_NAME}_R2_SCORE_log'
FIXED_ALPHA_VAL = 1e-6




def estimate_mean(shape_matrix, t, fn_mse, fn_r2):
    TOTAL_TIME_PTS = int(np.max(t))
    degree = 1
    # 1. Project onto PCA subspace
    N = t.shape[0]
    assert N == shape_matrix.shape[1]
    dM = shape_matrix.shape[0]

    shape_matrix_transformed = shape_matrix.T # sklearn pca on N X dM
    # ---> N X r
    r = shape_matrix_transformed.shape[1] # is actually dM
    print(f'r = {r} , N = {shape_matrix_transformed.shape[0]}, dM = {dM}')
    shape_matrix_transformed = shape_matrix_transformed.T # r X N
    
    betas_for_low_dim = estimate_parameters_from_pca_embeddings(shape_matrix_transformed, t, fn_mse, fn_r2)
    # --> r X (degree+1)
    assert betas_for_low_dim.shape[1] == (degree+1)

    mean_for_low_dim = np.zeros((r, N))
    # 2. Compute mean f(r) for low dim
    for i in range(N):
        vec = np.zeros(r)
        for p in range(degree+1):
            vec += (t[i]**p) * betas_for_low_dim[:, p]
        mean_for_low_dim[:, i] = vec

    # transform the mean back to original shape space
    transformed_mean = mean_for_low_dim.T
    # N X dM
    assert transformed_mean.shape[0] == N and transformed_mean.shape[1] == dM
    transformed_mean = transformed_mean.T
    assert transformed_mean.shape[0] == dM and transformed_mean.shape[1] == N
    return transformed_mean
    
def estimate_parameters_from_pca_embeddings(shape_matrix, t, fn_mse, fn_r2):
    print(f'----------ESTIMATING BETAS----------')
    N = t.shape[0]
    alpha_value = 1e-5
    
    assert N == shape_matrix.shape[1]
    TOTAL_TIME_PTS = int(np.max(t))
    degree = 1
    dM = shape_matrix.shape[0] 
    X = np.zeros((N, degree))

    for deg in range(0, degree):
        X[:, deg] = np.power(t, deg+1)
    # N X r
    shape_matrix = np.transpose(shape_matrix)
    betas = np.zeros((dM, degree+1))
    # compute polynomial for each mode

    lassoreg = LinearRegression()
    lassoreg.fit(X, shape_matrix)
    score_r2 = lassoreg.score(X, shape_matrix)
    with open(fn_r2, 'a') as f:
        f.write(f'{str(score_r2)}\n')
    print(f'################ R2 Score = {score_r2} #################') 
    betas[: ,0]= lassoreg.intercept_
    betas[: ,1:] = lassoreg.coef_
    print(f'----------BETAS ESTIMATED SUCCESSFULLY--------')
    
    return betas

    
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
