###########################################################################
# Description : Fully Automated Pipeline for Data data-augmentation
# Author: Riddhish Bhalodia, Shalin Parikh
# Institution : Scientific Computing and Imaging Institute
# Date : 6th May 2019
# Updates for ShapeWorks by Jadie Adams May 2020
###########################################################################

import numpy as np
from numpy import matlib as mb
import scipy as sp
import nrrd
import csv
import sys
import os
import xml.etree.ElementTree as ET
import re
import subprocess
import math
import numpy.matlib

'''
This file contains all the functions which are needed for the data augmentation process.
'''

def read_necessarry_metadata(img_list, point_list):
	'''
	Extract essential metadata for the rest of the algorithm
	'''
	N = len(img_list)
	pt = np.loadtxt(point_list[0])
	M = pt.shape[0]
	d = pt.shape[1]
	imgdims = np.array([0, 0, 0])
	nrdImg, f = nrrd.read(img_list[0])
	imgdims[0] = nrdImg.shape[0]
	imgdims[1] = nrdImg.shape[1]
	imgdims[2] = nrdImg.shape[2]

	return [N, M, d, imgdims, f]

def create_data_matrix(file_path,M,N,tag):
	'''
		create the pca matrix 
	'''
	if tag == 0:
		data_matrix = np.zeros([M, N])
		for i in range(len(file_path)):
			temp = np.loadtxt(file_path[i])
			data_matrix[..., i] = temp.reshape(1,M)
	else:
		data_matrix = np.zeros([M, N])
		for i in range(len(file_path)):
			[d,f] = nrrd.read(file_path[i])
			data_matrix[..., i] = d.reshape(1, M)
	return data_matrix

def get_mean_cov(pca_matrix,N):
	'''
		This returns the covariance matrix and the mean shape for PCA mode computation of the particles, pca_matrix is the created pca matrix; N is the number of inputs
	'''
	mean_shape = np.mean(pca_matrix, axis = 1)
	print(mean_shape.shape)
	pca_matrix = pca_matrix - numpy.matlib.repmat(mean_shape.reshape(mean_shape.shape[0], 1), 1, N)
	trick_cov_mat  = np.dot(pca_matrix.T,pca_matrix) * 1.0/np.sqrt(N-1)
	
	return trick_cov_mat, mean_shape, pca_matrix


def compute_eigen_vectors(trick_cov_mat,pca_matrix,N):
	'''
		Returns the eigen values and the normalized eigen vectors for PCA mode computation, trick_cov_matrix is the covariance matrix; pca_matrix is the created pca matrix; N is the number of inputs
	'''
	eigen_values, eigen_vectors = np.linalg.eigh(trick_cov_mat)
	new_eigen_vectors = np.dot(pca_matrix, eigen_vectors)
	for i in range(N):
		new_eigen_vectors[:,i] = new_eigen_vectors[:,i]/np.linalg.norm(new_eigen_vectors[:,i])
	eigen_values = np.flip(eigen_values)
	new_eigen_vectors = np.flip(new_eigen_vectors, 1)
	return eigen_values, new_eigen_vectors


def save_pca_information(mean_shape, eigen_values, eigen_vectors, parent_dir, M, d, N):

	'''
	Saves the PCA information in the appropriate directory
	'''
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

def save_pca_information_images(mean_shape, eigen_values, eigen_vectors, parent_dir, imgDims, N, f):

	'''
	Saves the PCA information in the appropriate directory
	'''
	newDir = os.path.join(parent_dir, 'PCA-Info-Images')
	if not os.path.exists(newDir):
		os.makedirs(newDir)
	meanNm = newDir + '/' + 'meanshape.nrrd'
	eigvalsNm = newDir + '/' + 'eigenvalues.txt'
	meanSh = mean_shape.reshape(imgDims[0], imgDims[1], imgDims[2])
	nrrd.write(meanNm, meanSh)
	np.savetxt(eigvalsNm, eigen_values)
	for i in range(N):
		nm = newDir + '/' + 'pcamode' + str(i) + '.nrrd'
		data = eigen_vectors[:, i]
		data = data.reshape(imgDims[0], imgDims[1], imgDims[2])
		nrrd.write(nm, data, f)


def get_data(path):
	'''
		Generic function to get all the files in a directory as a list
	'''
	file_list = []
	folder = os.listdir(path)
	for file in folder:
		file_list.append(file)
	return file_list


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


def create_python_xml(input_points_list, input_images_list, parent_dir):
	'''
		This creates a xml for the python code with <sample tags>
	'''
	root = ET.Element('sample')
	input_images = ET.SubElement(root, 'input_images')
	input_images.text = "\n"
	output_images = ET.SubElement(root, 'output_images')
	output_images.text = "\n"
	input_points = ET.SubElement(root, 'input_points')
	input_points.text = "\n"
	output_points = ET.SubElement(root, 'output_points')
	output_points.text = "\n"

	newDir = os.path.join(parent_dir, 'PCA-Info-Particles')
	meanNM = newDir + '/' + 'meanshape.particles'
	outputDir = os.path.join(parent_dir, 'TildeImages')
	if not os.path.exists(outputDir):
		os.makedirs(outputDir)

	output_images_list = []
	output_points_list = []
	for i in range(len(input_images_list)):
		temp = input_images_list[i]
		initPath = temp.rsplit('/', 1)[0]
		temp = temp.replace(initPath, outputDir)
		temp = temp.replace('.nrrd', '.tilde.nrrd')
		output_images_list.append(temp)
		output_points_list.append(meanNM)

	for i in range(len(input_points_list)):
		t1 = input_images.text
		t1 = t1 + input_images_list[i] + '\n'
		input_images.text = t1
		t2 = output_images.text
		t2 = t2 + output_images_list[i] + '\n'
		output_images.text = t2
		t3 = input_points.text
		t3 = t3 + input_points_list[i] + '\n'
		input_points.text = t3
		t4 = output_points.text
		t4 = t4 + output_points_list[i] + '\n'
		output_points.text = t4

	data = ET.tostring(root, encoding='unicode')
	# ET.dump(root)
	# print(data)
	file = open(parent_dir + "/XML_convert_to_tilde_python.xml", "w+")
	file.write(data)
	return output_images_list

def create_cpp_xml(filename, outputfilename):
	'''
		This creates a xml for cpp Shape warp binary
	'''
	opening_tag = "<"
	ending_tag = "</"
	closing_tag = ">"
	tree = ET.parse(filename)
	root = tree.getroot()
	children = {}
	for child in root:
		children[child.tag] = child.text
	tags = children.keys()
	xml_text = ""
	for tag in tags:
		xml_text += opening_tag+tag+closing_tag+children[tag]+ending_tag+tag+closing_tag
	file = open(outputfilename,"w")
	file.write(xml_text)
	file.close()

def warp_image_to_space(filename):
	subprocess.call(['./ShapeWarp', filename])


def pca_mode_loadings_computation_images(output_images_list, N, imgDims, parent_dir, cutoff, f):
	# data, header,x,y,z = read_data_from_nrrd(output_images_path)
	M = imgDims[0]*imgDims[1]*imgDims[2]
	pca_matrix = create_data_matrix(output_images_list,M,N,1)
	trick_cov_matrix, mean_shape, meanNor_data_matrix = get_mean_cov(pca_matrix,N)
	eigen_values, eigen_vectors = compute_eigen_vectors(trick_cov_matrix,pca_matrix,N) 
	cumDst = np.cumsum(eigen_values) / np.sum(eigen_values)
	newKpt = np.where(cumDst > cutoff)[0][0]
	k = newKpt + 1
	# eigen_values, new_eigen_vectors = compute_eigen_vectors(trick_cov_matrix,pca_matrix,N)	#Get the eigen values and normalized eigen vectors respectively
	save_pca_information_images(mean_shape, eigen_values, eigen_vectors, parent_dir, imgDims, N, f)
	W = eigen_vectors[:, :k]
	X_loadings = np.matmul(meanNor_data_matrix.T, W)
	return k, X_loadings, eigen_values, eigen_vectors, mean_shape


def compute_mahalanobis_distance(pca_loadings, path_to_eigen_values, num_modes):
	m,n = pca_loadings.shape
	md = []
	mean = np.mean(pca_loadings, axis = 0)
	std = np.std(pca_loadings, axis = 0)
	eigen_values = np.loadtxt(path_to_eigen_values+'eigen_values.txt')
	temp = pca_loadings - mean
	cov = np.zeros((num_modes,num_modes))
	for i in xrange(num_modes):
		for j in xrange(num_modes):
			if i == j:
				cov[i][j] = eigen_values[i]
	inv_cov = np.linalg.inv(cov)
	for i in xrange(m):
		temp2 = np.dot(np.dot(temp[i][:], inv_cov), temp[i][:].T)
		temp2 = math.sqrt(temp2)
		md.append(temp2)

	return md, cov, mean, std

def mahalDist(X, Data):
	mean = np.mean(Data, axis = 0)
	temp = X - mean
	cov = np.cov(Data.T) # this is pain
	dist = np.dot(np.dot(temp, np.linalg.inv(cov)), temp.T)
	return np.sqrt(dist)

def nearestImg(X, data):
	X = X / np.linalg.norm(X)
	for i in range(data.shape[0]):
		data[i, ...] = data[i, ...] / np.linalg.norm(data[i, ...])
	diffMat = np.sum((data - np.matlib.repmat(X, data.shape[0], 1))**2, axis = 1)
	return np.min(diffMat)

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
		# if projDist <= thresh:
		mulp = numpy.matlib.repmat(mulp, d*M, 1)
		Y = mean_shape + np.sum(mulp*use_eigvecs, 1)
		Ygen = Y.reshape(M, d)
		nm = newDir + '/Generated_sample_' + str(count) + '.particles'
		np.savetxt(nm, Ygen)
		path_list.append(nm)
		count += 1
	return path_list

def nearbyLoading(data):
	rdidx = np.random.randint(0, data.shape[0])
	newLoading = data[rdidx, ...] + np.random.randn(data.shape[1],)
	return newLoading

def generate_images(pca_loadings_images, eigvals_images, eigvecs_images, mean_shape, num_samples, K_img, imgDims, N, parent_dir, f):
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
		# if projDist <= thresh:
		print("Generated Image "+str(count+1) + " out of " + str(num_samples))
		mulp = numpy.matlib.repmat(mulp, imgSize, 1)
		Y = mean_shape + np.sum(mulp*use_eigvecs, 1)
		Ygen = Y.reshape(imgDims[0], imgDims[1], imgDims[2])
		nm = newDir + '/Generated_sample_' + str(count) + '.nrrd'
		nrrd.write(nm, Ygen, f)
		path_list.append(nm)
		count += 1
	return path_list

def findClosest(nm, pointList):
	print("find Closest")
	newPt = np.loadtxt(nm)
	minDst = 100000000000000000000000000000
	outIDX = 0
	for i in range(len(pointList)):
		ptTmp = np.loadtxt(pointList[i])
		dst = np.sum((ptTmp - newPt)**2)
		if dst < minDst:
			minDst = dst
			outIDX = i
	return outIDX

def create_final_xml_shape(num_samples, parent_dir, pointList, dataList):
	root = ET.Element('sample')
	input_images = ET.SubElement(root, 'input_images')
	input_images.text = "\n"
	output_images = ET.SubElement(root, 'output_images')
	output_images.text = "\n"
	input_points = ET.SubElement(root, 'input_points')
	input_points.text = "\n"
	output_points = ET.SubElement(root, 'output_points')
	output_points.text = "\n"

	newDirImages = os.path.join(parent_dir, 'Generated-Images')
	if not os.path.exists(newDirImages):
		os.makedirs(newDirImages)
	newDirParticles = os.path.join(parent_dir, 'Generated-Particles')
	
	output_images_list = []
	input_points_list = []
	output_points_list = []
	input_images_list = []
	print("Now compiling the xml file")
	for i in range(num_samples):
		nm = newDirParticles + '/Generated_sample_' + str(i) + '.particles'
		output_points_list.append(nm)
		idx = findClosest(nm, pointList)
		nm = newDirImages + '/Generated_sample_' + str(i) + '.nrrd'
		output_images_list.append(nm)
		input_points_list.append(pointList[idx])
		input_images_list.append(dataList[idx])

	for i in range(len(input_points_list)):
		t1 = input_images.text
		t1 = t1 + input_images_list[i] + '\n'
		input_images.text = t1
		t2 = output_images.text
		t2 = t2 + output_images_list[i] + '\n'
		output_images.text = t2
		t3 = input_points.text
		t3 = t3 + input_points_list[i] + '\n'
		input_points.text = t3
		t4 = output_points.text
		t4 = t4 + output_points_list[i] + '\n'
		output_points.text = t4

	data = ET.tostring(root, encoding='unicode')
	# ET.dump(root)
	# print(data)
	file = open("XML_get_final_images_python.xml", "w+")
	file.write(data)
	return output_images_list

def create_final_xml(num_samples, parent_dir):

	root = ET.Element('sample')
	input_images = ET.SubElement(root, 'input_images')
	input_images.text = "\n"
	output_images = ET.SubElement(root, 'output_images')
	output_images.text = "\n"
	input_points = ET.SubElement(root, 'input_points')
	input_points.text = "\n"
	output_points = ET.SubElement(root, 'output_points')
	output_points.text = "\n"

	newDirImagesTilde = os.path.join(parent_dir, 'Generated-Tilde-Images')
	newDirImages = os.path.join(parent_dir, 'Generated-Images')
	if not os.path.exists(newDirImages):
		os.makedirs(newDirImages)
	newDirParticles = os.path.join(parent_dir, 'Generated-Particles')

	newDir = os.path.join(parent_dir, 'PCA-Info-Particles')
	meanNM = newDir + '/' + 'meanshape.particles'
	
	output_images_list = []
	input_points_list = []
	output_points_list = []
	input_images_list = []
	for i in range(num_samples):
		input_points_list.append(meanNM)
		nm = newDirParticles + '/Generated_sample_' + str(i) + '.particles'
		output_points_list.append(nm)
		nm = newDirImagesTilde + '/Generated_sample_' + str(i) + '.nrrd'
		input_images_list.append(nm)
		nm = newDirImages + '/Generated_sample_' + str(i) + '.nrrd'
		output_images_list.append(nm)

	for i in range(len(input_points_list)):
		t1 = input_images.text
		t1 = t1 + input_images_list[i] + '\n'
		input_images.text = t1
		t2 = output_images.text
		t2 = t2 + output_images_list[i] + '\n'
		output_images.text = t2
		t3 = input_points.text
		t3 = t3 + input_points_list[i] + '\n'
		input_points.text = t3
		t4 = output_points.text
		t4 = t4 + output_points_list[i] + '\n'
		output_points.text = t4

	data = ET.tostring(root, encoding='unicode')
	# ET.dump(root)
	# print(data)
	file = open(parent_dir + "/XML_get_final_images_python.xml", "w+")
	file.write(data)
	return output_images_list
