###########################################################################
# Description : Functions for Data data-augmentation
# Author: Riddhish Bhalodia, Shalin Parikh
# Institution : Scientific Computing and Imaging Institute
# Date : 6th May 2019
# Updated for ShapeWorks by Jadie Adams May 2020
###########################################################################
import numpy as np
import numpy.matlib
import subprocess
import itk
import csv
import sys
import os
import re

'''
Extracts essential metadata for the rest of the algorithm
'''
def read_necessarry_metadata(img_list, point_list):
	N = len(img_list)
	pt = np.loadtxt(point_list[0])
	M = pt.shape[0]
	d = pt.shape[1]
	itk_image = itk.imread(img_list[0])
	nrdImg = itk.array_from_image(itk_image)
	return [N, M, d, nrdImg.shape]

########################### PCA code ########################################

'''
pca_mode_loading_computation and pca_mode_loadings_computation_images helper
	create the pca matrix 
'''
def create_data_matrix(file_path,M,N,tag):
	if tag == 0:
		data_matrix = np.zeros([M, N])
		for i in range(len(file_path)):
			temp = np.loadtxt(file_path[i])
			data_matrix[..., i] = temp.reshape(1,M)
	else:
		data_matrix = np.zeros([M, N])
		for i in range(len(file_path)):
			itk_image = itk.imread(file_path[i], itk.F)
			np_view = itk.array_view_from_image(itk_image)
			d = itk.array_from_image(itk_image)
			data_matrix[..., i] = d.reshape(1, M)
	return data_matrix

'''
pca_mode_loading_computation and pca_mode_loadings_computation_images helper
	This returns the covariance matrix and the mean shape for PCA mode computation of the particles, pca_matrix is the created pca matrix; N is the number of inputs
'''
def get_mean_cov(pca_matrix,N):
	mean_shape = np.mean(pca_matrix, axis = 1)
	pca_matrix = pca_matrix - numpy.matlib.repmat(mean_shape.reshape(mean_shape.shape[0], 1), 1, N)
	trick_cov_mat  = np.dot(pca_matrix.T,pca_matrix) * 1.0/np.sqrt(N-1)
	return trick_cov_mat, mean_shape, pca_matrix

'''
pca_mode_loading_computation and pca_mode_loadings_computation_images helper
	Returns the eigen values and the normalized eigen vectors for PCA mode computation, 
	trick_cov_matrix is the covariance matrix; pca_matrix is the created pca matrix; N is the number of inputs
'''
def compute_eigen_vectors(trick_cov_mat,pca_matrix,N):
	eigen_values, eigen_vectors = np.linalg.eigh(trick_cov_mat)
	new_eigen_vectors = np.dot(pca_matrix, eigen_vectors)
	for i in range(N):
		new_eigen_vectors[:,i] = new_eigen_vectors[:,i]/np.linalg.norm(new_eigen_vectors[:,i])
	eigen_values = np.flip(eigen_values)
	new_eigen_vectors = np.flip(new_eigen_vectors, 1)
	return eigen_values, new_eigen_vectors

'''
pca_mode_loading_computation helper
	Saves the PCA information in the appropriate directory
'''
def save_pca_information(mean_shape, eigen_values, eigen_vectors, parent_dir, M, d, N):
	newDir = os.path.join(parent_dir, 'PCA-Info-Particles')
	if not os.path.exists(newDir):
		os.makedirs(newDir)
	meanNm = newDir + '/' + 'meanshape.particles'
	eigvalsNm = newDir + '/' + 'eigenvalues.txt'
	meanSh = mean_shape.reshape(M, d)
	np.savetxt(meanNm, meanSh)
	np.savetxt(eigvalsNm, eigen_values)
	for i in range(N):
		nm = newDir + '/' + 'pcamode' + str(i) + '.particles'
		data = eigen_vectors[:, i]
		data = data.reshape(M, d)
		np.savetxt(nm, data)

'''
pca_mode_loadings_computation_images helper
	Saves the PCA information in the appropriate directory
'''
def save_pca_information_images(mean_shape, eigen_values, eigen_vectors, parent_dir, imgDims, N):
	newDir = os.path.join(parent_dir, 'PCA-Info-Images')
	if not os.path.exists(newDir):
		os.makedirs(newDir)
	meanNm = newDir + '/' + 'meanshape.nrrd'
	eigvalsNm = newDir + '/' + 'eigenvalues.txt'
	meanSh = mean_shape.reshape(imgDims)
	meanSh = np.float32(meanSh)  #itk won't work with float64 for some reason
	itk_np_view = itk.image_view_from_array(meanSh)
	itk_np_copy = itk.image_from_array(meanSh)
	itk.imwrite(itk_np_copy, meanNm)
	np.savetxt(eigvalsNm, eigen_values)
	for i in range(N):
		nm = newDir + '/' + 'pcamode' + str(i) + '.nrrd'
		data = eigen_vectors[:, i]
		data = data.reshape(imgDims[0], imgDims[1], imgDims[2])
		data = np.float32(data) #itk won't work with float64 for some reason
		itk_np_view = itk.image_view_from_array(data)
		itk_np_copy = itk.image_from_array(data)
		itk.imwrite(itk_np_view, nm)

'''
	Computes eigenvualues, eigenvectors, and mean shape for particles
'''
def pca_mode_loading_computation(file_path, M, N, d, parent_dir, cutoff):
	pca_matrix = create_data_matrix(file_path,M*d,N,0)	
	trick_cov_matrix, mean_shape, meanNor_data_matrix = get_mean_cov(pca_matrix,N)	
	eigen_values, eigen_vectors = compute_eigen_vectors(trick_cov_matrix,pca_matrix,N) 
	cumDst = np.cumsum(eigen_values) / np.sum(eigen_values)
	newKpt = np.where(cumDst > cutoff)[0][0]
	k_pt = newKpt + 1
	save_pca_information(mean_shape, eigen_values, eigen_vectors, parent_dir, M, d, N)
	# now compute the PCA loadings
	W = eigen_vectors[:, :k_pt]
	X_loadings = np.matmul(meanNor_data_matrix.T, W)
	return k_pt, X_loadings, eigen_values, eigen_vectors, mean_shape

'''
	Computes eigenvualues, eigenvectors, and mean shape for images
'''
def pca_mode_loadings_computation_images(output_images_list, N, imgDims, parent_dir, cutoff):
	M = imgDims[0]*imgDims[1]*imgDims[2]
	pca_matrix = create_data_matrix(output_images_list,M,N,1)
	trick_cov_matrix, mean_shape, meanNor_data_matrix = get_mean_cov(pca_matrix,N)
	eigen_values, eigen_vectors = compute_eigen_vectors(trick_cov_matrix,pca_matrix,N) 
	cumDst = np.cumsum(eigen_values) / np.sum(eigen_values)
	newKpt = np.where(cumDst > cutoff)[0][0]
	k = newKpt + 1
	save_pca_information_images(mean_shape, eigen_values, eigen_vectors, parent_dir, imgDims, N)
	W = eigen_vectors[:, :k]
	X_loadings = np.matmul(meanNor_data_matrix.T, W)
	return k, X_loadings, eigen_values, eigen_vectors, mean_shape

################################### Warp Code ###############################################

'''
	Warps input image by transform from input points to mean
'''
def warp_image_to_mean(input_points_list, input_images_list, parent_dir):
	meanNM = os.path.join(parent_dir, 'PCA-Info-Particles') + '/meanshape.particles'
	outputDir = os.path.join(parent_dir, 'TildeImages')
	if not os.path.exists(outputDir):
		os.makedirs(outputDir)
	output_images_list = []
	for i in range(len(input_images_list)):
		print("Warping " + str( i+1) + " out of " + str(len(input_images_list)))
		input_image = input_images_list[i]
		input_particles = input_points_list[i]
		temp = input_images_list[i]
		initPath = temp.rsplit('/', 1)[0]
		temp = temp.replace(initPath, outputDir)
		output_image_name = temp.replace('.nrrd', '.tilde.nrrd')
		output_images_list.append(output_image_name)
		cmd = ["shapeworks", 
			"read-image", "--name", input_image]
		cmd.extend(["warp-image", "--source_landmarks", input_particles, "--target_landmarks", meanNM])
		cmd.extend(["write-image", "--name", output_image_name])
		subprocess.check_call(cmd)
	return output_images_list

'''
	Warps input image by transform from mean to output points
'''
def warp_mean_to_image(output_points_list, input_images_list, parent_dir):
	meanNM = os.path.join(parent_dir, 'PCA-Info-Particles') + '/meanshape.particles'
	outputDir = os.path.join(parent_dir, 'Generated-Images')
	if not os.path.exists(outputDir):
		os.makedirs(outputDir)
	output_images_list = []
	for i in range(len(input_images_list)):
		print("Warping " + str( i+1) + " out of " + str(len(input_images_list)))
		input_image = input_images_list[i]
		output_particles = output_points_list[i]
		temp = input_images_list[i]
		initPath = temp.rsplit('/', 1)[0]
		temp = temp.replace(initPath, outputDir)
		output_image_name = temp.replace('.tilde.nrrd', '.nrrd')
		output_images_list.append(output_image_name)
		cmd = ["shapeworks", 
			"read-image", "--name", input_image]
		cmd.extend(["warp-image", "--source_landmarks", meanNM, "--target_landmarks", output_particles])
		cmd.extend(["write-image", "--name", output_image_name])
		subprocess.check_call(cmd)
	return output_images_list

########################################## Generate Code #################################################

'''
	generate_particles helper - gets mahal dist
'''
def mahalDist(X, Data):
	mean = np.mean(Data, axis = 0)
	temp = X - mean
	cov = np.cov(Data.T) # this is pain
	dist = np.dot(np.dot(temp, np.linalg.inv(cov)), temp.T)
	return np.sqrt(dist)

'''
	generate_particles helper - gets nearest image
'''
def nearestImg(X, data):
	X = X / np.linalg.norm(X)
	for i in range(data.shape[0]):
		data[i, ...] = data[i, ...] / np.linalg.norm(data[i, ...])
	diffMat = np.sum((data - np.matlib.repmat(X, data.shape[0], 1))**2, axis = 1)
	return np.min(diffMat)

'''
	Gets particles from PCA
'''
def generate_particles(pca_loadings_particles, eigvals_particles, eigvecs_particles, mean_shape, num_samples, K_pt, M, d, N, parent_dir):
	path_list = []
	use_eigvals = eigvals_particles[:K_pt]
	use_sd = np.sqrt(use_eigvals)
	use_eigvecs = eigvecs_particles[:, :K_pt]
	newDir = os.path.join(parent_dir, 'Generated-Particles')
	if not os.path.exists(newDir):
		os.makedirs(newDir)
	count = 0
	AllLoadings = np.zeros([num_samples, K_pt])
	while count < num_samples:
		print("Generated Patricles " +str(count+1)+ " out of " + str(num_samples))
		rv = 0.2*np.random.randn(1, K_pt)
		mulp = rv*use_sd
		projDist = mahalDist(mulp, pca_loadings_particles)
		mulp = numpy.matlib.repmat(mulp, d*M, 1)
		Y = mean_shape + np.sum(mulp*use_eigvecs, 1)
		Ygen = Y.reshape(M, d)
		nm = newDir + '/Generated_sample_' + str(count) + '.particles'
		np.savetxt(nm, Ygen)
		path_list.append(nm)
		count += 1
	return path_list

''' 
	Gets image from PCA 
'''
def generate_images(pca_loadings_images, eigvals_images, eigvecs_images, mean_shape, num_samples, K_img, imgDims, N, parent_dir):
	path_list = []
	use_eigvals = eigvals_images[:K_img]
	use_sd = np.sqrt(use_eigvals)
	use_eigvecs = eigvecs_images[:, :K_img]
	imgSize = imgDims[0]*imgDims[1]*imgDims[2]
	newDir = os.path.join(parent_dir, 'Generated-Tilde-Images')
	if not os.path.exists(newDir):
		os.makedirs(newDir)
	count = 0
	AllLoadings = np.zeros([num_samples, K_img])
	while count < num_samples:
		rv = np.random.randn(1, K_img)
		mulp = rv*use_sd
		projDist = nearestImg(mulp, pca_loadings_images)
		print("Generated Image "+str(count+1) + " out of " + str(num_samples))
		mulp = numpy.matlib.repmat(mulp, imgSize, 1)
		Y = mean_shape + np.sum(mulp*use_eigvecs, 1)
		Ygen = Y.reshape(imgDims[0], imgDims[1], imgDims[2])
		nm = newDir + '/Generated_sample_' + str(count) + '.nrrd'
		Ygen = np.float32(Ygen) #itk doesn't work with float64 for some reason
		itk_np_view = itk.image_view_from_array(Ygen)
		itk_np_copy = itk.image_from_array(Ygen)
		itk.imwrite(itk_np_view, nm)
		path_list.append(nm)
		count += 1
	return path_list
