# Embedder Module
# Jadie Adams
# July 2020
# Embedder objects take a data matrix and embed it into a lower dimensional space
import os
import numpy as np
from abc import ABC, abstractmethod
from shapeworks.utils import sw_message

# abstract base class for embedders 
class Embedder(ABC):
	# abstract method
	def __init__(self, data_matrix):
		self.data_matrix = data_matrix
	def getEmbeddedMatrix(self):
		pass
	def project(self, PCA_instance):
		pass
 
# instance of embedder that uses PCA for dimension reduction
class PCA_Embbeder(Embedder):
	# overriding abstract methods
	def __init__(self, data_matrix, num_dim=0, percent_variability=0.95):
		self.data_matrix = data_matrix
		num_dim = self.run_PCA(num_dim, percent_variability)
		self.num_dim = num_dim
	# run PCA on data_matrix for PCA_Embedder
	def run_PCA(self, num_dim, percent_variability):
		# get covariance matrix (uses compact trick)
		N = self.data_matrix.shape[0]
		data_matrix_2d = self.data_matrix.reshape(self.data_matrix.shape[0], -1).T # flatten data instances and transpose
		mean = np.mean(data_matrix_2d, axis=1)
		centered_data_matrix_2d = (data_matrix_2d.T - mean).T
		trick_cov_matrix  = np.dot(centered_data_matrix_2d.T,centered_data_matrix_2d) * 1.0/np.sqrt(N-1)
		# get eignevectors and eigenvalues
  
		# Check if percent_variability is within valid range
		if percent_variability < 0 or percent_variability > 100:
			percent_variability = 100

		eigen_values, eigen_vectors = np.linalg.eigh(trick_cov_matrix)
		eigen_vectors = np.dot(centered_data_matrix_2d, eigen_vectors)
		for i in range(N):
			eigen_vectors[:,i] = eigen_vectors[:,i]/np.linalg.norm(eigen_vectors[:,i])
		eigen_values = np.flip(eigen_values)
		eigen_vectors = np.flip(eigen_vectors, 1)
		# get num PCA components
		cumDst = np.cumsum(eigen_values) / np.sum(eigen_values)
		if num_dim == 0:
			cumDst = np.cumsum(eigen_values) / np.sum(eigen_values)
			num_dim = np.where(cumDst > float(percent_variability))
			if num_dim and len(num_dim[0]) > 0:
				num_dim = num_dim[0][0] + 1
			else:
				num_dim = len(cumDst)
		W = eigen_vectors[:, :num_dim]
		PCA_scores = np.matmul(centered_data_matrix_2d.T, W)
		sw_message(f"The PCA modes of particles being retained : {num_dim}")
		sw_message(f"Variablity preserved: {str(float(cumDst[num_dim-1]))}")
		self.num_dim = num_dim
		self.PCA_scores = PCA_scores
		self.eigen_vectors = eigen_vectors
		self.eigen_values = eigen_values
		return num_dim

	# write PCA info to files 
	# @TODO do we need all of this?
	def write_PCA(self, out_dir, suffix):
		if not os.path.exists(out_dir):
			os.makedirs(out_dir)
		np.save(out_dir +  'original_PCA_scores.npy', self.PCA_scores)
		mean = np.mean(self.data_matrix, axis=0)
		np.savetxt(out_dir + 'mean.' + suffix, mean)
		np.savetxt(out_dir + 'eigenvalues.txt', self.eigen_values)
		for i in range(self.data_matrix.shape[0]):
			nm = out_dir + 'pcamode' + str(i) + '.' + suffix
			data = self.eigen_vectors[:, i]
			data = data.reshape(self.data_matrix.shape[1:])
			np.savetxt(nm, data)
	# returns embedded form of dtat_matrix
	def getEmbeddedMatrix(self):
		return self.PCA_scores
	# projects embbed array into data
	def project(self, PCA_instance):
		W = self.eigen_vectors[:, :self.num_dim].T
		mean = np.mean(self.data_matrix, axis=0)
		data_instance =  np.matmul(PCA_instance, W) + mean.reshape(-1)
		data_instance = data_instance.reshape((self.data_matrix.shape[1:]))
		return data_instance