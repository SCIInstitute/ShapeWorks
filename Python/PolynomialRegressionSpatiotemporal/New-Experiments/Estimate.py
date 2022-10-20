import sklearn.decomposition
from sklearn.linear_model import MultiTaskLassoCV, Lasso
import numpy as np

def run_pca(shape_matrix):
    pca_ob = sklearn.decomposition.PCA(svd_solver='auto', n_components=2)
    shape_matrix_transformed = pca_ob.fit_transform(shape_matrix)
    scree = pca_ob.explained_variance_ratio_.cumsum()
    minDims = np.where(scree < 0.9999999)[0][-1]
    eigvecs = pca_ob.components_
    print(f"--------Number of modes covering 97% varaince - {minDims} -------")
    return shape_matrix_transformed, minDims, eigvecs, pca_ob

def estimate_mean(shape_matrix, t, fn_mse='temp_mse.txt', fn_r2 = 'temp_r2.txt'):
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
    
    betas_for_low_dim = estimate_parameters_from_pca_embeddings(shape_matrix_transformed, t)
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
    
def estimate_parameters_from_pca_embeddings(shape_matrix, t, alpha_value=1e-5, fn_mse='temp_mse.txt', fn_r2 = 'temp_r2.txt'):
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

    lassoreg = MultiTaskLassoCV()
    lassoreg.fit(X, shape_matrix)
    print(f'-----Lasso Fit done alpha chosen {lassoreg.alpha_}')
    z_hat = lassoreg.predict(X)
    mse = np.mean((z_hat - shape_matrix)**2)
    rel_mse = mse/np.mean((mean_shape - shape_matrix)**2)
    print(f'################ Relative MSE = {rel_mse} #################')
    with open(fn_mse, 'a') as f:
        f.write(f'{str(rel_mse)}\n')
    score_r2 = lassoreg.score(X, shape_matrix)
    with open(fn_r2, 'a') as f:
        f.write(f'{str(score_r2)}\n')
    print(f'################ R2 Score = {score_r2} #################')
    betas = np.zeros((dM, degree+1))
    betas[:,0]= lassoreg.intercept_
    betas[:,1:] = lassoreg.coef_
    print(f'----------BETAS ESTIMATED SUCCESSFULLY--------')
    return betas


def estimate_mean_only_lasso(shape_matrix, t, fn_mse='temp_mse.txt', fn_r2 = 'temp_r2.txt'):
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
    
    betas_for_low_dim = estimate_parameters_from_pca_embeddings_only_lasso(shape_matrix_transformed, t)
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


def estimate_parameters_from_pca_embeddings_only_lasso(shape_matrix, t, alpha_value=1e-5, fn_mse='temp_mse.txt', fn_r2 = 'temp_r2.txt'):
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

    lassoreg = Lasso(alpha=alpha_value)
    lassoreg.fit(X, shape_matrix)
    # print(f'-----Lasso Fit done alpha chosen {lassoreg.alpha_}')
    z_hat = lassoreg.predict(X)
    mse = np.mean((z_hat - shape_matrix)**2)
    rel_mse = mse/np.mean((mean_shape - shape_matrix)**2)
    print(f'################ Relative MSE = {rel_mse} #################')
    with open(fn_mse, 'a') as f:
        f.write(f'{str(rel_mse)}\n')
    score_r2 = lassoreg.score(X, shape_matrix)
    with open(fn_r2, 'a') as f:
        f.write(f'{str(score_r2)}\n')
    print(f'################ R2 Score = {score_r2} #################')
    betas = np.zeros((dM, degree+1))
    betas[:,0]= lassoreg.intercept_
    betas[:,1:] = lassoreg.coef_
    print(f'----------BETAS ESTIMATED SUCCESSFULLY--------')
    return betas