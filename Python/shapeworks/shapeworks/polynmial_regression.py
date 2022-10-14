import shapeworks as sw
import numpy as np
from sklearn.model_selection import KFold
from sklearn.linear_model import Lasso

ALPHA_VALUES = [1e-5, 1e-4] # Add more

def estimate_parameters(shape_matrix, t, alpha_value):
    print(f'----------ESTIMATING BETAS----------')
    N = t.shape[0]
    degree = 23
    for deg in range(0, degree):
        X[:, deg] = np.pow(t, deg+1)

    X = np.zeros(N, degree)
    lassoreg = Lasso(alpha=alpha_value, normalize=True, max_iter=100000)
    lassoreg.fit(X, shape_matrix)
    lassoreg.fit(X, shape_matrix)
    z_hat = lassoreg.predict(X)
    mse = np.mean((z_hat - shape_matrix)**2)
    print(mse)
    betas = np.zeros((M*d, degree+1))
    betas[:,0]= lassoreg.intercept_
    betas[:,1:] = lassoreg.coef_
    print(f'----------BETAS ESTIMATED SUCCESSFULLY--------')
    return betas

def get_optimum_alpha(shape_matrix, t):
    print(f'----------FINDING OPTIMUM ALPHA VALUE----------')
    # run k-fold estimate of parameters and return optimum alpha_value
    for alpha_val in ALPHA_VALUES:
        pass

    print(f'----------OPTIMUM ALPHA VALUE = {0}----------')
    return 0