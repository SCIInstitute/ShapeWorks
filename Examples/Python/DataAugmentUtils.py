import os
import numpy as np 
from numpy import matlib
import itk
import subprocess

def RunDataAugmentation(out_dir, img_list, point_list, num_samples, num_PCA, sample_type="KDE"):
	pca_particle_scores, mean_particles, eigvecs_particles, M_particles = ProcessOriginal(out_dir, img_list, point_list, num_PCA)
	# Make output folders
	generated_PCA_scores = []
	gen_point_dir = out_dir + "Generated-Particles/"
	makeDir(gen_point_dir)
	gen_point_paths = []
	gen_image_dir = out_dir + "Generated-Images/"
	makeDir(gen_image_dir)
	gen_image_paths = []
	# Get sigma squared for sampling
	if sample_type=="KDE":
		sigma_squared = getKDEsigma(pca_particle_scores)
	else:
		sigma_squared = getGaussianSigma(pca_particle_scores)
	W = np.transpose(eigvecs_particles)[:num_PCA]
	# Generate
	for index in range(1, num_samples+1):
		# Get name index
		name_index = str(index)
		while len(name_index) < 4:
			name_index = "0" + name_index
		# sample PCA score
		if sample_type=="KDE":
			new_PCA_score, base_image_path, base_particles_path = sampleKDE(img_list, point_list, pca_particle_scores, sigma_squared)
		else:
			new_PCA_score, base_image_path, base_particles_path = sampleGaussian(img_list, point_list, pca_particle_scores, sigma_squared)
		generated_PCA_scores.append(new_PCA_score)
		# get and save new particles
		new_particles =  np.matmul(np.array([new_PCA_score]), W) + np.matlib.repmat(mean_particles, 1, 1)
		new_particles = new_particles.reshape((M_particles,3))
		gen_particles_path = gen_point_dir + 'Generated_sample_' + name_index + '.particles'
		np.savetxt(gen_particles_path, new_particles)
		gen_point_paths.append(gen_particles_path)
		# get new image
		gen_image_path = GenerateImage(out_dir, gen_particles_path, base_image_path, base_particles_path)
		gen_image_paths.append(gen_image_path)
		print("Generated: " + str(index) + "/" + str(num_samples))
	np.save(os.path.join(out_dir, 'generated_PCA_scores.npy'), np.array(generated_PCA_scores))
	# write to csv
	csv_out = open(out_dir + "totalData.csv", "a")
	for gen_index in range(num_samples):
		string = gen_image_paths[gen_index] + "," + gen_point_paths[gen_index]
		for score in generated_PCA_scores[gen_index]:
			string += "," + str(score)
		csv_out.write(string + "\n")
	csv_out.close()
	return out_dir + "totalData.csv", out_dir + 'original_PCA_scores.npy'

def ProcessOriginal(out_dir, img_list, point_list, num_PCA):
	# Initialize
	makeDir(out_dir)
	N_images = len(img_list)
	pt = np.loadtxt(point_list[0])
	M_particles = pt.shape[0]
	pt_dim = pt.shape[1]
	# Get orig particle PCA loadings
	K_pt, pca_particle_scores, eigvecs_particles, mean_particles = pca_mode_loading_computation(point_list, M_particles, N_images, pt_dim, out_dir, num_PCA)
	np.save(os.path.join(out_dir, 'original_PCA_scores.npy'), pca_particle_scores)
	print("The PCA modes of particles being retained : ", K_pt)
	# Write original data to csv
	csv_out = open(out_dir + "totalData.csv", "w+")
	for orig_index in range(N_images):
		string = img_list[orig_index] + "," + point_list[orig_index]
		for score in pca_particle_scores[orig_index]:
			string += "," + str(score)
		csv_out.write(string + "\n")
	csv_out.close()
	return pca_particle_scores, mean_particles, eigvecs_particles, M_particles

def getKDEsigma(pca_particle_scores):
	nearest_neighbor_dists = []
	cov = np.cov(pca_particle_scores.T) # this is pain
	for score_i in pca_particle_scores:
		smallest = np.Inf
		for score_j in pca_particle_scores:
			temp = score_i - score_j
			dist = np.dot(np.dot(temp, np.linalg.inv(cov)), temp.T) #mahalanobis
			if dist < smallest and dist != 0:
				smallest = dist
		nearest_neighbor_dists.append(smallest)
	return np.mean(np.array(nearest_neighbor_dists))

def sampleKDE(img_list, point_list, pca_particle_scores, sigma_squared):
	# Get random base example
	base_index = np.random.randint(len(img_list))
	base_image_path = img_list[base_index]
	base_particles_path = point_list[base_index]
	base_PCA_scores = pca_particle_scores[base_index]
	# Get PCA noise
	noise = []
	for i in range(pca_particle_scores.shape[1]):
		noise.append(np.random.normal(0,sigma_squared))
	noise = np.array(noise)
	# get new pca score
	new_PCA_score = base_PCA_scores + noise
	return new_PCA_score, base_image_path, base_particles_path

# @TDOD
def getGaussianSigma(pca_particle_scores):
	return 1

# @TODO
def sampleGaussian(img_list, point_list, pca_particle_scores, sigma_squared):
	base_image_path = ''
	base_particles_path = ''
	new_PCA_score = []
	return new_PCA_score, base_image_path, base_particles_path

'''
Use warp between particles to warp original image into a new image
'''
def GenerateImage(out_dir, gen_particles, base_image, base_particles):
	image_name = gen_particles.split('/')[-1].replace(".particles",".nrrd")
	gen_image = out_dir + "Generated-Images/" + image_name
	cmd = ["shapeworks", 
		"read-image", "--name", base_image,
		"warp-image", "--source", base_particles, "--target", gen_particles,
		"write-image", "--name", gen_image]
	subprocess.check_call(cmd)
	return gen_image

'''
Compute PCA from original example particles
'''
def pca_mode_loading_computation(file_list, M, N, d, parent_dir, k_pt):
	pca_matrix = create_data_matrix(file_list,M*d,N,0)	
	trick_cov_matrix, mean_shape, meanNor_data_matrix = get_mean_cov(pca_matrix,N)	
	eigen_values, eigen_vectors = compute_eigen_vectors(trick_cov_matrix,pca_matrix,N)
	cumDst = np.cumsum(eigen_values) / np.sum(eigen_values)
	print("Variablity preserved: " + str(cumDst[k_pt-1]))
	if k_pt == 0:
		cumDst = np.cumsum(eigen_values) / np.sum(eigen_values)
		newKpt = np.where(cumDst > 0.95)[0][0]
		k_pt = newKpt + 1
	save_pca_information(mean_shape, eigen_values, eigen_vectors, parent_dir, M, d, N)
	# Compute the PCA scores
	W = eigen_vectors[:, :k_pt]
	X_loadings = np.matmul(meanNor_data_matrix.T, W)
	return k_pt, X_loadings, eigen_vectors, mean_shape

'''
pca_mode_loading_computation helper
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
pca_mode_loading_computation helper
'''
def get_mean_cov(pca_matrix,N):
	mean_shape = np.mean(pca_matrix, axis = 1)
	pca_matrix = pca_matrix - np.matlib.repmat(mean_shape.reshape(mean_shape.shape[0], 1), 1, N)
	trick_cov_mat  = np.dot(pca_matrix.T,pca_matrix) * 1.0/np.sqrt(N-1)
	return trick_cov_mat, mean_shape, pca_matrix

'''
pca_mode_loading_computation helper
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
Make folder
'''
def makeDir(dirPath):
    if not os.path.exists(dirPath):
        os.makedirs(dirPath)