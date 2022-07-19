from npd import isPD, nearestPD
import numpy as np
import torch
from timeit import default_timer as timer
import seaborn as sns
from matplotlib import pyplot as plt
import pandas as pd
import xml.etree.ElementTree as ET



from Constants import DEVICE

def log_multivariate_normal_density(X, means, covars, min_covar=1.e-6):
    """Log probability for full covariance matrices. """
    # print(f'Computing Log-Probability now, sizes are X= {X.size()}, means = {means.size()}, covars = {covars.size()}')
    n_samples, n_dim = X.size()
    nmix = len(means)
    log_prob = torch.empty(n_samples, nmix).to(DEVICE)
    start_log_time = timer()
    for c, (mu, cv) in enumerate(zip(means, covars)):
        
        # print(f'c={c}, cv.size() = {cv.size()} \n cv = \n{cv}')
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

    log_prob_filtered = torch.nan_to_num(log_prob)
    return log_prob_filtered

def repeat(times, M):
    result = []
    for i in range(times):
        result.append(M)
    result = np.array(result)
    return result

def plot_box_plots_new(file_1, file_2, label_1, label_2, xlabel, ylabel, plot_name, legend_name='Model Type'):
    plt.style.use('plot_style.txt')
    model_A_rmse = np.loadtxt(file_1)
    model_B_rmse = np.loadtxt(file_2)
    assert model_A_rmse.shape == model_B_rmse.shape
    rmse_cols = np.zeros(2 * model_A_rmse.shape[0] * model_A_rmse.shape[1])
    percent_missing_cols = []
    model_type_cols = []
    P, T_S = model_A_rmse.shape
    start_idx = 0
    missing_time_points = [4, 8, 12, 16, 20]

    for p in range(0, P):
        model_A_rmses_at_t = model_A_rmse[p, :]
        percent_val = f'{str(round(missing_time_points[p]/25, 2)*100)}%'
        p_col = [percent_val] * T_S
        model_col = [f'{label_1}'] * T_S
        percent_missing_cols += p_col
        model_type_cols += model_col
        rmse_cols[start_idx:start_idx+T_S] = model_A_rmses_at_t
        start_idx = start_idx + T_S
    assert start_idx == (P * T_S)
    for p in range(0, P):
        model_B_rmses_at_t = model_B_rmse[p, :]
        percent_val = f'{str(round(missing_time_points[p]/25, 2)*100)}%'
        p_col = [percent_val] * T_S
        model_col = [f'{label_2}'] * T_S
        percent_missing_cols += p_col
        model_type_cols += model_col
        rmse_cols[start_idx:start_idx+T_S] = model_B_rmses_at_t
        start_idx = start_idx + T_S

    df = pd.DataFrame(list(zip(percent_missing_cols, rmse_cols, model_type_cols)), columns=[f'{xlabel}', f'{ylabel}', f'{legend_name}'])
    fig = plt.gcf()
    fig.set_size_inches(22.5, 10.5)
    ax = sns.boxplot(x=xlabel, y=ylabel, hue=legend_name, data=df, linewidth=2.5)
    plt.savefig(plot_name)




def create_analyze_xml(xmlfilename, particle_files):
    with open(xmlfilename, 'w') as file:
        file.write('<point_files>\n')
        for fn in particle_files:
            file.write(f'{fn}\n')
        file.write('</point_files>')
    print('XML File Created====')