from shapeworks import ParticleSystem
import numpy as np
from hotelling.stats import hotelling_t2
import statsmodels.stats.multitest as multi
from shapeworks.utils import sw_message
from shapeworks.utils import sw_progress
from shapeworks.utils import sw_check_abort
from scipy import stats
'''
This function calculates the p-values per correspondence point for the group difference.
Input are:
data - pandas data frame with two columns file name and group ids
number_of_particles - number of correspondence particles 
permutations - number of times the pvalues are calculated
'''


def compute_pvalues_for_group_difference(data, number_of_particles, permutations=100):
    group_id = data["group_ids"].unique()
    group_0_filenames = list(data[data["group_ids"] == group_id[0]]["filename"])
    group_1_filenames = list(data[data["group_ids"] == group_id[1]]["filename"])

    group_0_data = ParticleSystem(group_0_filenames).Particles()
    group_0_data = np.reshape(group_0_data, (number_of_particles, 3, -1))
    group_1_data = ParticleSystem(group_1_filenames).Particles()
    group_1_data = np.reshape(group_1_data, (number_of_particles, 3, -1))

    return compute_pvalues_for_group_difference_data(group_0_data, group_1_data, permutations)

def compute_pvalues_for_group_difference_studio(group_0_data, group_1_data, permutations=100):
    number_of_particles = int(group_0_data.shape[0] / 3)
    group_0 = np.reshape(group_0_data, (number_of_particles, 3, -1))
    group_1 = np.reshape(group_1_data, (number_of_particles, 3, -1))
    return compute_pvalues_for_group_difference_data(group_0, group_1, permutations)

def compute_pvalues_for_group_difference_data(group_0_data, group_1_data, permutations=100):
    number_of_particles = group_0_data.shape[0]
    group_0_size = group_0_data.shape[-1]
    group_1_size = group_1_data.shape[-1]
    subset_size = min(group_0_size, group_1_size)

    pvalues_matrix = np.zeros((number_of_particles, permutations))
    idx = 0
    for p in range(permutations):
        sw_progress(float(idx) / permutations)
        idx = idx + 1
        if sw_check_abort():
            sw_message("Aborted")
            return

        group_0_index = np.random.choice(group_0_size, subset_size, replace=False)
        group_1_index = np.random.choice(group_1_size, subset_size, replace=False)

        group_0_subset = group_0_data[:, :, group_0_index]
        group_1_subset = group_1_data[:, :, group_1_index]

        for particle_id in range(number_of_particles):
            x = group_0_subset[particle_id, :, :].T
            y = group_1_subset[particle_id, :, :].T

            stats, f, pv, s = hotelling_t2(x, y)
            pvalues_matrix[particle_id, p] = pv
    corrected_pvalue_matrix = np.zeros((number_of_particles, 1))
    for particle_id in range(number_of_particles):
        r, pval = multi.fdrcorrection(pvalues_matrix[particle_id, :], alpha=0.05)
        corrected_pvalue_matrix[particle_id, 0] = np.mean(pval)
    return corrected_pvalue_matrix

def get_estimate_dof(data):
    N,d = data.shape 
    dof = []
    for i in range(d):
        dof.append(stats.t.fit(data[:,i])[2])
    return np.mean(np.array(dof))


def robust_ppca(data,latent_dim,iters=1000,lr=1e-3,alph0=100,alpha1=0.1):
    N,d = data.shape
    dof = get_estimate_dof(data) 
    
    nu = torch.Tensor([dof])
    mu = torch.Tensor([np.mean(data,axis=0)]).reshape((1,-1))
    W = torch.rand(d,latent_dim)
    sigma2 = torch.Tensor([1])
    log_un = torch.ones(N)*2

    data = torch.from_numpy(data).float()
    data.requires_grad = False
    nu.requires_grad = True 
    mu.requires_grad = True
    W.requires_grad = True
    sigma2.requires_grad = True
    log_un.requires_grad = True

    un = torch.exp(log_un)
    eps = 1e-5
    
    tn = torch.ones((N,latent_dim,1 ))
    tn.requires_grad = False
    loss_array = []
    if(d==2):
        alpha0 = 0
        opt = torch.optim.SGD([W,sigma2,log_un,mu], lr=lr)
        opt_nu = torch.optim.SGD([nu],lr=1e-4)
        rate = 1
    else:
        rate = -1*iters/np.log(alpha1)
        opt = torch.optim.SGD([W,sigma2,log_un,mu], lr=lr)
        opt_nu = torch.optim.SGD([nu],lr=1e-3)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(opt_nu, 'min')
    
    for ite in range(iters):
        alpha = alpha0*np.exp(-ite/rate)

        un = torch.exp(log_un)

        tn = torch.bmm(torch.mm(torch.pinverse(torch.mm(W.t().clone(),W.clone()) + \
             sigma2*torch.eye(latent_dim) + alpha*torch.ones(latent_dim,latent_dim)),W.t().clone()).repeat(N,1,1),torch.unsqueeze(torch.sub(data,mu),2).clone())

        W2 = W.t().clone().repeat(N,1,1)
        
        B = torch.unsqueeze(torch.sub(data,mu),2).clone()

        term1 = N*(d/2) * torch.log(sigma2 + eps) 

        new_data = torch.unsqueeze(torch.sub(data,mu),2)
        
        term2 = ((0.5/sigma2) * torch.sum(un*torch.bmm(torch.transpose(new_data,1,2),new_data)[:,0,0]))[0]

        term3_1 = ((1/sigma2)*un.view(-1,1,1))*tn
        
        term3_2 = torch.bmm(W2,B)
        
        term3 = (torch.bmm(torch.transpose(term3_1,1,2),term3_2)).sum()

        term4 = (1/sigma2)* (torch.diagonal(torch.bmm(torch.mm(W.t().clone(),W).repeat(N,1,1),torch.bmm(un.view(-1,1,1)*tn,torch.transpose(tn,1,2))), dim1=-2, dim2=-1).sum(-1) ).sum()
        
        term5 = N*torch.sum((nu/2)*torch.log((nu/2)+eps) - (nu/2).lgamma())
        
        term6 = torch.sum((0.5*nu - 1)*torch.log(un+eps) - (nu/2)*un)

        loss = torch.stack([term1[0] ,term2, -1*term3,term4[0] ,term5 , term6 ]).sum()
        loss = loss/(N*d)

        opt.zero_grad()
        opt_nu.zero_grad()
        loss.backward(retain_graph=True)
        opt.step()
        opt_nu.step()
        loss_array.append(loss.detach().numpy())
        scheduler.step(loss)

        matrix = np.matmul(W.T,W)
        eigen_values, rotation_matrix = np.linalg.eig(matrix)
        eigen_vector_W = np.matmul(W,rotation_matrix)
        exp_var = np.cumsum(eigen_values)/ (np.sum(eigen_values) + sigma2)
        X_minusMean = data - mu

        return eigen_values_W,eigen_vector_W,mu,exp_var,X_minusMean


def get_rrpcaEigenVector(data,latent_dim,iters=1000,lr=1e-3,alph0=100,alpha1=0.1): 
    eigen_values_W,eigen_vector_W,mu,exp_var,_X_minusMean = robust_ppca(data,latent_dim,iters,lr,alph0,alpha1)
    return eigen_vector_W

def get_rppcaEigenValues(data,latent_dim,iters=1000,lr=1e-3,alph0=100,alpha1=0.1): 
    eigen_values_W,eigen_vector_W,mu,exp_var,_X_minusMean = robust_ppca(data,latent_dim,iters,lr,alph0,alpha1)
    return eigen_values_W

def get_expVar(data,latent_dim,iters=1000,lr=1e-3,alph0=100,alpha1=0.1): 
    eigen_values_W,eigen_vector_W,mu,exp_var,X_minusMean = robust_ppca(data,latent_dim,iters,lr,alph0,alpha1)
    return exp_var

def get_rppcamean(data,latent_dim,iters=1000,lr=1e-3,alph0=100,alpha1=0.1): 
    eigen_values_W,eigen_vector_W,mu,exp_var,X_minusMean = robust_ppca(data,latent_dim,iters,lr,alph0,alpha1)
    return mu

def get_X_minsMean(data,latent_dim,iters=1000,lr=1e-3,alph0=100,alpha1=0.1): 
    eigen_values_W,eigen_vector_W,mu,exp_var,X_minusMean = robust_ppca(data,latent_dim,iters,lr,alph0,alpha1)
    return X_minusMean