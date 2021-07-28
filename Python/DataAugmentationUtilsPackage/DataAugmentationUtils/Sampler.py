# Sampler Module
# Jadie Adams
# July 2020
# Sampler objects create a distribution based on an embedded matrix of data and sample from it.
import os
import numpy as np
from abc import ABC, abstractmethod
from sklearn.mixture import GaussianMixture
import random
from shapeworks.utils import sw_message


###################### Sampler Class ###################################

# abstract base class for samplers
class Sampler(ABC):
	# abstract methods
	# fit should fit a distribution to the embedded_matrix
	def fit(self, embedded_matrix):
		self.embedded_matrix = embedded_matrix
		pass
	# sample should return the sample and the index of the nearest real example
	def sample(self):
		pass

# instance of Sampler class that uses a single Gaussian
class Gaussian_Sampler(Sampler):
	def fit(self, embedded_matrix):
		sw_message("Fitting Gaussian distribution...")
		self.embedded_matrix = embedded_matrix
		self.mean = np.mean(embedded_matrix, axis=0)
		self.cov = np.cov(embedded_matrix, rowvar=0)
	def sample(self):
		sample = np.random.multivariate_normal(self.mean, self.cov)
		closest_index = getClosest(sample, self.embedded_matrix)
		return sample, closest_index

# instance of Sampler class that uses a mixture of Gaussians
# mixture_num is the number of clusters to use (if 0 it autoselects the best number using the elbow method)
class Mixture_Sampler(Sampler):
	def fit(self, embedded_matrix, mixture_num):
		sw_message("Fitting Gaussian mixture model...")
		self.embedded_matrix = embedded_matrix
		if mixture_num == 0:
			mixture_num = self.selectClusterNum()
		self.GMM = GaussianMixture(mixture_num, covariance_type='full', random_state=0)
		self.GMM.fit(self.embedded_matrix)
		sw_message("Gaussian mixture model converged: " + str(self.GMM.converged_))
	# get optimal cluster number by minimizing Akaike information criterion (AIC) and Bayesian information criterion (BIC)
	def selectClusterNum(self):
		n_components = np.arange(1, self.embedded_matrix.shape[1])
		models = [GaussianMixture(n, covariance_type='full', random_state=0).fit(self.embedded_matrix) for n in n_components]
		bic_min_index = np.argmin(np.array([m.bic(self.embedded_matrix) for m in models]))
		aic_min_index = np.argmin(np.array([m.aic(self.embedded_matrix) for m in models]))
		avg_index = int((bic_min_index + aic_min_index) / 2)
		mixture_num = n_components[avg_index]
		sw_message("Using " + str(mixture_num) + " components.")
		return mixture_num
	def sample(self):
		rand_int = random.randint(1, 1000)
		sample = self.GMM.sample(rand_int)[0][-1]
		closest_index = getClosest(sample, self.embedded_matrix)
		return sample, closest_index

# instance of Sampler class that uses kernel density estimate
class KDE_Sampler(Sampler):
	def fit(self, embedded_matrix):
		sw_message("Fitting KDE...")
		self.embedded_matrix = embedded_matrix
		# get sigma squared
		nearest_neighbor_dists = []
		cov = np.cov(embedded_matrix.T)
		for i in embedded_matrix:
			smallest = np.Inf
			for j in embedded_matrix:
				dist = Mdist(i,j,cov)
				if dist < smallest and dist != 0:
					smallest = dist
			nearest_neighbor_dists.append(smallest)
		self.sigma_squared = np.mean(np.array(nearest_neighbor_dists))/embedded_matrix.shape[1]
	def sample(self):
		base_index = np.random.randint(self.embedded_matrix.shape[0])
		base_PCA_score = self.embedded_matrix[base_index]
		noise = []
		for i in range(self.embedded_matrix.shape[1]):
			noise.append(np.random.normal(0,self.sigma_squared))
		noise = np.array(noise)
		sampled_PCA_score = base_PCA_score + noise
		return sampled_PCA_score, base_index

###################### Helper methods ###################################

# sampler helper - gets mahalanobis distance
def Mdist(instance_i, instance_j, covariance_matrix):
	temp = instance_i - instance_j
	dist = np.dot(np.dot(temp, np.linalg.inv(covariance_matrix)), temp.T)
	return dist

# sampler helper - gets closest real example to sample
def getClosest(sample, embedded_matrix):
	covariance_matrix = np.cov(embedded_matrix.T)
	smallest = np.inf
	for index in range(embedded_matrix.shape[0]):
		example = embedded_matrix[index]
		dist = Mdist(sample, example, covariance_matrix)
		if dist <= smallest:
			smallest = dist
			closest = index
	return closest
