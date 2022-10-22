import sklearn.decomposition
from sklearn.linear_model import MultiTaskLassoCV, Lasso, MultiTaskElasticNetCV, LinearRegression, MultiTaskElasticNet, ElasticNet
import numpy as np

def run_pca(shape_matrix):
    pca_ob = sklearn.decomposition.PCA(svd_solver='full')
    shape_matrix_transformed = pca_ob.fit_transform(shape_matrix)
    scree = pca_ob.explained_variance_ratio_.cumsum()
    minDims = np.where(scree < 0.9999999)[0][-1]
    eigvecs = pca_ob.components_
    print(f"--------Number of modes covering 97% varaince - {minDims} -------")

    return shape_matrix_transformed, minDims, eigvecs, pca_ob

def estimate_mean(shape_matrix, t, fn_mse, fn_r2):
    TOTAL_TIME_PTS = int(np.max(t))
    degree = TOTAL_TIME_PTS - 1
    # 1. Project onto PCA subspace
    N = t.shape[0]
    assert N == shape_matrix.shape[1]
    dM = shape_matrix.shape[0]

    shape_matrix_transformed, _, _, pca_ob = run_pca(shape_matrix.T) # sklearn pca on N X dM
    # ---> N X r
    r = shape_matrix_transformed.shape[1]
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
    transformed_mean = pca_ob.inverse_transform(mean_for_low_dim.T)
    # N X dM
    assert transformed_mean.shape[0] == N and transformed_mean.shape[1] == dM
    transformed_mean = np.ascontiguousarray(transformed_mean)
    transformed_mean = transformed_mean.T
    assert transformed_mean.shape[0] == dM and transformed_mean.shape[1] == N
    return transformed_mean
    
def estimate_parameters_from_pca_embeddings(shape_matrix, t, fn_mse, fn_r2):
    print(f'----------ESTIMATING BETAS----------')
    N = t.shape[0]
    alpha_value = 1e-5
    
    assert N == shape_matrix.shape[1]
    TOTAL_TIME_PTS = int(np.max(t))
    degree = TOTAL_TIME_PTS - 1
    # degree = 1
    dM = shape_matrix.shape[0] 
    X = np.zeros((N, degree))

    for deg in range(0, degree):
        X[:, deg] = np.power(t, deg+1)
    # N X r
    shape_matrix = np.transpose(shape_matrix)
    betas = np.zeros((dM, degree+1))
    # compute polynomial for each mode
    for r in range(shape_matrix.shape[1]):
        lassoreg = ElasticNet()
        lassoreg.fit(X, shape_matrix[:, r])
        score_r2 = lassoreg.score(X, shape_matrix[:, r])
        with open(fn_r2, 'a') as f:
            f.write(f'{str(score_r2)}\n')
        print(f'################ R2 Score = {score_r2} #################') 
        betas[r,0]= lassoreg.intercept_
        betas[r,1:] = lassoreg.coef_
        print(f'----------BETAS ESTIMATED SUCCESSFULLY--------')
    
    return betas
