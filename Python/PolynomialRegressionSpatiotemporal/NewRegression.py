import numpy as np
from sklearn.model_selection import KFold
from sklearn.linear_model import Lasso, LassoCV, MultiTaskLassoCV
from sklearn.linear_model import LinearRegression
import glob
import os
from pathlib import Path
import json

MODELS_WORKING_DIR = '/home/sci/nawazish.khan/Public/Spatiotemporal-Polynomial-Regression-Experiments/'


def estimate_parameters(shape_matrix, t, alpha_value, fn_mse='temp_mse.txt', fn_r2 = 'temp_r2.txt'):
    print(f'----------ESTIMATING BETAS----------')
    N = t.shape[0]
    assert N == shape_matrix.shape[1]
    TOTAL_TIME_PTS = int(np.max(t))
    degree = TOTAL_TIME_PTS - 1
    # degree = 1
    dM = shape_matrix.shape[0]
    X = np.zeros((N, degree))

    for deg in range(0, degree):
        X[:, deg] = np.power(t, deg+1)
    
    shape_matrix = np.transpose(shape_matrix)
    mean_shape = np.tile(np.mean(shape_matrix, axis=0), (N,1))
    # print(f'New shape _matrix shape {shape_matrix.shape} and X shape = {X.shape}')

    reg = Lasso(alpha=alpha_value)
    # lienar_reg = LinearRegression()
    # lassoreg = MultiTaskLassoCV(alphas=ALPHA_VALUES)
    reg.fit(X, shape_matrix)
    # print(f'-----Lasso Fit done alpha chosen {lassoreg.alpha_}')
    z_hat = reg.predict(X)
    # print(f'z_hat shape {z_hat.shape}')
    mse = np.mean((z_hat - shape_matrix)**2)
    rel_mse = mse/np.mean((mean_shape - shape_matrix)**2)
    print(f'################ Relative MSE = {rel_mse} #################')
    with open(fn_mse, 'a') as f:
        f.write(f'{str(rel_mse)}\n')
    score_r2 = reg.score(X, shape_matrix)
    with open(fn_r2, 'a') as f:
        f.write(f'{str(score_r2)}\n')
    print(f'################ R2 Score = {score_r2} #################')
    betas = np.zeros((dM, degree+1))
    betas[:,0]= reg.intercept_
    betas[:,1:] = reg.coef_
    print(f'----------BETAS ESTIMATED SUCCESSFULLY--------')
    return betas