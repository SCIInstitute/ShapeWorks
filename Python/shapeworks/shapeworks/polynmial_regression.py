from turtle import shape
import shapeworks as sw
import numpy as np
from hotelling.stats import hotelling_t2
import statsmodels.stats.multitest as multi
from shapeworks.utils import sw_message
from shapeworks.utils import sw_progress
from shapeworks.utils import sw_check_abort
from scipy import stats
from sklearn.model_selection import KFold
from sklearn.linear_model import Lasso
from sklearn.linear_model import LinearRegression
T = 25


def estimate_parameters(shape_matrix, t, alpha_value):
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
    return betas

