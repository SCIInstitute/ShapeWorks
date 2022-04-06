import shapeworks as sw
import numpy as np
from hotelling.stats import hotelling_t2
import statsmodels.stats.multitest as multi
from shapeworks.utils import sw_message
from shapeworks.utils import sw_progress
from shapeworks.utils import sw_check_abort
from scipy import stats
from sklearn.preprocessing import MinMaxScaler



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

    group_0_data = sw.ParticleSystem(group_0_filenames).Particles()
    group_0_data = np.reshape(group_0_data, (number_of_particles, 3, -1))
    group_1_data = sw.ParticleSystem(group_1_filenames).Particles()
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



def calculate_minVariance(matrix):
    eigen_values,eigen_vectors = np.linalg.eigh(matrix)
    eigen_values = sorted(eigen_values,reverse=False)
    
    explained_variance = sorted(eigen_values/sum(eigen_values),reverse=True)
    cumulative_variance = np.array(explained_variance).cumsum()
    
    min_dims = np.where(cumulative_variance <=0.90)[0]
    print("min_dims: ", min_dims)
    #if the first mode is the most dominant, min_dims will be empty
    if(min_dims.size==0):
        min_dims = 1
    else:
        min_dims = min_dims[-1]+1
    if(min_dims==matrix.shape[1]):
        min_dims = min_dims -1
    
    
    m_MinimumVariance = np.sum(np.absolute(eigen_values[min_dims:]))
    return m_MinimumVariance

class WPPCA():
    def __init__(self,x,latent_dim=2,num_iterations=10):


        '''
        Data will be stored as a coloumn matrix. 
        Each column is a sample and each row is a feature
        feature_dim: d
        latent_dim: q
        '''
        num_samples,feature_dim = x.shape
        self.feature_dim = feature_dim
        self.num_samples = num_samples

        self.x = x.T
        self.num_iterations = num_iterations
        self.latent_dim = latent_dim
        
        '''
        Parameters of Weighted PPCA
        W: d x q
        mu: d
        sigma2: scalar
        weights: N weights which have a beta distribution
        a,b: prior of beta distribution Beta(0.1, 0.01)
        '''     
        self.weights = np.ones(num_samples)
        self.mu = np.mean(self.x,axis=1).reshape((-1,1))
        self.W = np.random.random((self.feature_dim,self.latent_dim))
        self.sigma2 = 1
        # self.a = 0.1
        # self.b = 0.1
        self.a = self.num_samples/1e5
        self.b = self.num_samples/1e6

    def standarize(self):
        self.mean = np.mean(self.x, axis=1).reshape((-1,1))
        self.x = self.x - self.mean
        # calculate standard deviation
        std = np.std(self.x, axis=1).reshape((-1,1))
        # divide by standard deviation
        

        if np.all(std != 0):
            self.x /= std

    def get_expectation_terms(self,W,mu,sigma2,weights=None):
        '''
        Expectation terms from from equation (55) and (56) from Mixtures of Probabilistic Principal Component Analysers(MPPCA)
        E_tn : q x N
        E_tn_tnT: q x q x N
        This function returns E_tn[q x N] and sum(E_tn_tnT)[q x q]
        '''
        x = self.x
        if(weights is None):
            weights = np.ones(self.num_samples)

        M = W.T.dot(W) + sigma2*np.eye(self.latent_dim)
        Minv = np.linalg.inv(M)

        E_tn = Minv.dot(W.T).dot((x-mu))
        
        repeated_M = np.repeat(sigma2*Minv[np.newaxis, :, :], self.num_samples, axis=0)
        term1 =  (np.broadcast_to(weights, repeated_M.T.shape).T) * repeated_M
        term2 = E_tn.dot(np.multiply(E_tn.T,weights[:,np.newaxis]))
        weighted_E_tn_tnT = np.sum(term1,axis=0) + term2
        
        return Minv,E_tn,weighted_E_tn_tnT

    '''
    Per sample likelihood calculation will require per sample expectation terms
    '''
    def get_expectation_terms_per_sample(self,W,mu,sigma2,weights=None):
        '''
        Expectation terms from from equation (55) and (56) from Mixtures of Probabilistic Principal Component Analysers(MPPCA)
        E_tn : q x N
        E_tn_tnT: q x q x N
        '''
        x = self.x
        if(weights is None):
            weights = np.ones(self.num_samples)

        M = W.T.dot(W) + sigma2*np.eye(self.latent_dim)
        Minv = np.linalg.inv(M)

        E_tn = np.zeros((self.latent_dim,self.num_samples))
        weighted_E_tn_tnT = np.zeros((self.latent_dim,self.latent_dim,self.num_samples))
        data_minus_mean = x-mu
        for i in range(self.num_samples):
            E_tn[:,i] = Minv.dot(W.T).dot((data_minus_mean[:,i]))
            weighted_E_tn_tnT[:,:,i] = (sigma2*Minv)+ E_tn[:,i].dot(E_tn[:,i].T)
            weighted_E_tn_tnT[:,:,i] *= weights[i]
        
        return Minv,E_tn,weighted_E_tn_tnT

    '''
    Need the individual likelihood terms for calculation of weight update
    For weight update, see equation 8 from Robust Probabilistic Modeling with Bayesian Data Reweighting
    for gamma prior on weights
    For beta prior, a quadratic equation will have to be solved
    '''

    def get_likelihood(self,E_tn_sample,weighted_E_tn_tnT_sample,mu,W,sigma2):
        x = self.x
        L = np.zeros(self.num_samples)
        data_minus_mean = x-mu
        norm_term = np.square(np.linalg.norm(data_minus_mean,axis=0)) 
        for i in range(self.num_samples):
            weighted_E_tn_tnT = weighted_E_tn_tnT_sample[:,:,i]
            E_tn = E_tn_sample[:,i]

            single_l = self.feature_dim*0.5*np.log(sigma2)+ \
                0.5*np.trace(weighted_E_tn_tnT) + \
                (0.5/sigma2)*norm_term[i] - \
                (1/sigma2)*(E_tn.T.dot(np.transpose(W)).dot(data_minus_mean[:,i])) +\
                (0.5/sigma2)*np.trace(weighted_E_tn_tnT.dot(W.T.dot(W)))

            L[i] = -1*single_l

        return L

    def wppca_em(self):

        # self.standarize()
        L_old = -1000000000
        for i in range(self.num_iterations):
            '''
            E STEP
            Compute the expectation with respect to latent variable distribution marginal 
            p(t/x,W,sigma2,mu)
            Distribution is defined in Equaton 8: Mixtures of Probabilistic Principal Component Analysers(MPPCA)
            M: q x q
            '''
            
            
            Minv,E_tn,weighted_E_tn_tnT_sample = self.get_expectation_terms_per_sample(self.W,self.mu,self.sigma2,self.weights)
            weighted_E_tn_tnT = np.mean(weighted_E_tn_tnT_sample,axis=2)
            
            

            '''
            M STEP
            Log likelihood is maximized with respect to W,sigma2,mu,weights. 
            Log likelihood = weights*equation(58) from MPPCA + log(prob distribution of weights)
                           = log(p(w)) + sum over N samples {wn * p(xn/tn)*p(tn)}
            For update equations, see Nonlinear feature extraction for soft sensor modeling based on weighted probabilistic PCA
            Equation 17 and 18
            '''

            '''
            update mean
            '''
            new_mu = np.multiply((self.x-self.W.dot(E_tn)),self.weights[np.newaxis,:])
            new_mu = np.sum(new_mu,axis=1)/np.sum(self.weights)
            new_mu = new_mu.reshape((-1,1))


            
            '''
            update W matrix
            '''
            data_minus_mean = self.x - new_mu
            weighted_data = np.multiply(data_minus_mean,self.weights[np.newaxis,:])
            m_MinimumVariance = calculate_minVariance(weighted_E_tn_tnT)
            print(m_MinimumVariance)
            W_new = (weighted_data.dot(E_tn.T)).dot(np.linalg.inv(weighted_E_tn_tnT + m_MinimumVariance))
            
            '''
            update sigma
            '''

            norm_term = np.square(np.linalg.norm(data_minus_mean,axis=0)) * self.weights[np.newaxis,:]
            norm_term_sum = np.sum(norm_term)   
            
            sigma2_new = (1 / (np.sum(self.weights) * self.feature_dim)) * \
                        ((norm_term_sum-\
                        2 * np.trace(E_tn.T.dot(np.transpose(W_new)).dot(weighted_data)))+\
                        np.trace(weighted_E_tn_tnT.dot(W_new.T.dot(W_new))))

            sigma2_new = np.absolute(sigma2_new)
            print("Sigma2: ",sigma2_new)

            '''
            update weights
            '''

            L = self.get_likelihood(E_tn,weighted_E_tn_tnT_sample,new_mu,W_new,sigma2_new)/self.num_samples#/self.feature_dim
            # plt.figure()
            # plt.bar(list(range(self.num_samples)),L)
            # plt.show()
            

            coeff = np.zeros((self.num_samples,3))
            coeff[:,0] = L
            coeff[:,1] = self.a + self.b - L - 2
            coeff[:,2] = 1 - self.a

            weights_new = self.weights*0
            for ids in range(self.num_samples):
                roots = np.roots(coeff[ids,:]) 
                print(roots)
                weights_new[ids] = roots[0]
            weights_new = np.absolute(weights_new)
            # scaler = MinMaxScaler(feature_range=(0.2, 0.9))
            # un_array = weights_new.reshape(-1,1)
            # weights_new = scaler.fit_transform(un_array)[:,0] 


            '''
            calculate total likelihood
            '''
            _,E_tn_sample,E_tn_tnT_sample = self.get_expectation_terms_per_sample(W_new,new_mu,sigma2_new)
            
            L = self.get_likelihood(E_tn_sample,E_tn_tnT_sample,new_mu,W_new,sigma2_new)
            total_likelihood = np.sum((self.a-1)*np.log(weights_new) + np.log(1-weights_new)*(1-self.b)  + weights_new*L)
            total_likelihood = total_likelihood/self.num_samples
            print("Total Likelihood: ",total_likelihood)
            
            if(total_likelihood>L_old):
            # if(True):
                L_old = total_likelihood
                self.weights = weights_new
                self.W = np.copy(W_new)
                self.sigma2 = np.copy(sigma2_new)
                self.mu = np.copy(new_mu.reshape((-1,1)))
                
                
            else:
                
                break
            

        self.W = self.W/np.linalg.norm(self.W,axis=0)
        matrix = np.matmul(self.W.T,self.W)

        eigen_values, rotation_matrix = np.linalg.eigh(matrix)
        self.W = np.matmul(self.W,rotation_matrix)
        self.eigen_values = eigen_values
        index = self.eigen_values.argsort()[::-1]
        self.eigen_values = self.eigen_values[index]
        eigen_vector_W = self.W[:,index]
        norms = np.linalg.norm(eigen_vector_W,axis=0)
        self.W = eigen_vector_W
        
            

def get_rrpcaEigenVector(data,latent_dim,iters=10): 
    # eigen_values_W,eigen_vector_W,mu,exp_var,X_minusMean = robust_ppca(data,latent_dim,iters,lr,alpha0,alpha1)
    wppca = WPPCA(data.T,latent_dim,iters)
    wppca.wppca_em()

    return wppca.W

def get_rppcaEigenValues(data,latent_dim,iters=10): 
    # eigen_values_W,eigen_vector_W,mu,exp_var,X_minusMean = robust_ppca(data,latent_dim,iters,lr,alpha0,alpha1)
    wppca = WPPCA(data.T,latent_dim,iters)
    wppca.wppca_em()
    
    return wppca.eigen_values
    

def get_expVar(data,latent_dim,iters=10): 
    # eigen_values_W,eigen_vector_W,mu,exp_var,X_minusMean = robust_ppca(data,latent_dim,iters,lr,alpha0,alpha1)
    wppca = WPPCA(data.T,latent_dim,iters)
    wppca.wppca_em()
    expVar = wppca.eigen_values*100/np.sum(wppca.eigen_values)
    
    return expVar

def get_rppcamean(data,latent_dim,iters=10): 
    # eigen_values_W,eigen_vector_W,mu,exp_var,X_minusMean = robust_ppca(data,latent_dim,iters,lr,alpha0,alpha1)
    wppca = WPPCA(data.T,latent_dim,iters)
    wppca.wppca_em()
    return wppca.mu



def normalize(subj_map, group1_mean_map, group2_mean_map):
    slope = (2.0 / (group2_mean_map - group1_mean_map))
    subj_diff = subj_map - group1_mean_map
    subj_map_normalized = slope * subj_diff - 1
    return subj_map_normalized


def lda_loadings(group1_data, group2_data):
    group1_num = np.shape(group1_data)[1]
    group2_num = np.shape(group2_data)[1]

    combined_data = np.concatenate((group1_data, group2_data), axis=1)
    group1_mean = np.mean(group1_data, axis=1)
    group2_mean = np.mean(group2_data, axis=1)

    overall_mean = np.mean(combined_data, axis=1)

    diffVect = group1_mean - group2_mean

    group1_mean_diff = group1_mean - overall_mean
    group2_mean_diff = group2_mean - overall_mean

    group1_mean_map = np.dot(diffVect, group1_mean_diff)
    group2_mean_map = np.dot(diffVect, group2_mean_diff)

    group1_mean_map_normalized = normalize(group1_mean_map, group1_mean_map, group2_mean_map)
    group2_mean_map_normalized = normalize(group2_mean_map, group1_mean_map, group2_mean_map)

    group1_map = np.zeros((group1_num,))
    group2_map = np.zeros((group2_num,))

    for ii in range(group1_num):
        subjDiff = group1_data[:, ii] - overall_mean
        group1_map[ii] = np.dot(diffVect, subjDiff)
        group1_map[ii] = normalize(group1_map[ii], group1_mean_map, group2_mean_map)

    for ii in range(group2_num):
        subjDiff = group2_data[:, ii] - overall_mean
        group2_map[ii] = np.dot(diffVect, subjDiff)
        group2_map[ii] = normalize(group2_map[ii], group1_mean_map, group2_mean_map)

    group1_map_mean = group1_map.mean()
    group2_map_mean = group2_map.mean()

    group1_map_std = group1_map.std()
    group2_map_std = group2_map.std()

    group1_x = np.linspace(group1_map_mean - 6, group1_map_mean + 6, num=300)
    group2_x = np.linspace(group2_map_mean - 6, group2_map_mean + 6, num=300)

    group1_pdf = stats.norm.pdf(group1_x, group1_map_mean, group1_map_std)
    group2_pdf = stats.norm.pdf(group2_x, group2_map_mean, group2_map_std)
    return group1_x, group2_x, group1_pdf, group2_pdf, group1_map, group2_map


def lda(data):
    group_id = data["group_ids"].unique()
    group1_idxs = data.index[data['group_ids'] == 0].tolist()
    group2_idxs = data.index[data['group_ids'] == 1].tolist()
    filenames = data["filename"]
    # Provide the list of file names
    particle_data = sw.ParticleSystem(filenames)

    # Calculate the PCA for the read particle system
    shape_statistics = sw.ParticleShapeStatistics()
    shape_statistics.PCA(particleSystem=particle_data, domainsPerShape=1)

    # Calculate the loadings
    shape_statistics.principalComponentProjections()
    pca_loadings = shape_statistics.pcaLoadings().T
    group1_data = pca_loadings[:, group1_idxs]
    group2_data = pca_loadings[:, group2_idxs]

    return lda_loadings(group1_data, group2_data)
