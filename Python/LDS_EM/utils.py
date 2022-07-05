from npd import isPD, nearestPD
import numpy as np
import torch
from timeit import default_timer as timer

DEVICE = 'cuda:0'
# torch.set_default_dtype(torch.float64)
def log_multivariate_normal_density(X, means, covars, min_covar=1.e-6):
    """Log probability for full covariance matrices. """

    n_samples, n_dim = X.size()
    nmix = len(means)
    log_prob = torch.empty(n_samples, nmix).to(DEVICE)
    start_log_time = timer()
    for c, (mu, cv) in enumerate(zip(means, covars)):
        try:
            cv_chol, info = torch.linalg.cholesky_ex(cv, check_errors=True)
        except RuntimeError:
            # The model is most probabily stuck in a component with too
            # few observations, we need to reinitialize this components
            # print('inside log prob exception')
            # print('Finding nearest PSD')
            mat_A = cv + min_covar * torch.eye(n_dim).to(DEVICE)
            # if not isPD(mat_A):
            mat_A = nearestPD(mat_A)
            cv_chol, info = torch.linalg.cholesky_ex(mat_A, check_errors=True)
        cv_log_det = 2 * torch.sum(torch.log(torch.diagonal(cv_chol))) # 0 dim tensor
        try:
            cv_sol = torch.triangular_solve((X - mu).T, cv_chol, upper=False).solution.T # (AX=b) args(b,A)
        except:
            raise RuntimeError('Linear Solve Triangular Failed')
        # except ValueError:
        #     cv_sol = np.linalg.solve(cv_chol, (X - mu).T).T
        log_prob[:, c] = - .5 * (torch.sum(cv_sol ** 2, 1) + n_dim * torch.log(torch.tensor(2 * np.pi)) + cv_log_det)
    end_log_time = timer()
    # print(f'---- Log Probability computed in {end_log_time-start_log_time} seconds ------')
    return log_prob


def repeat(times, M):
    result = []
    for i in range(times):
        result.append(M)
    result = np.array(result)
    return result