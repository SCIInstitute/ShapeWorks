# Data Augmentation Pipelines
# Jadie Adams
# July 2020
# Contains two different data augmentation pipleines called by the RunDataAugmentation method
import os
import subprocess
import numpy as np
 
from DataAugmentationUtils import Embedder
from DataAugmentationUtils import Sampler

################################# Augmentaiton Pipelines ###############################################

def point_based_aug(out_dir, orig_img_list, orig_point_list, num_samples, num_PCA=0, sampler_type="KDE", mixture_num=0):
	# Get Embedder
	point_matrix = create_data_matrix(orig_point_list)
	PointEmbedder = Embedder.PCA_Embbeder(point_matrix, num_PCA)
	PointEmbedder.write_PCA(out_dir + "PCA_Particle_Info/", "particles") # write PCA info for DeepSSM testing
	embedded_matrix = PointEmbedder.getEmbeddedMatrix()
	# Get sampler
	if sampler_type == "Gaussian":
		PointSampler = Sampler.Gaussian_Sampler()
		PointSampler.fit(embedded_matrix) 
	elif sampler_type == "mixture":
		PointSampler = Sampler.Mixture_Sampler()
		PointSampler.fit(embedded_matrix, mixture_num) 
	elif sampler_type == "KDE":
		PointSampler = Sampler.KDE_Sampler()
		PointSampler.fit(embedded_matrix) 
	else:
		print("Error sampler_type unrecognized.")
		print("Gaussian, mixture, and KDE currently supported.")
	
	# Initialize output folders and lists
	gen_point_dir = out_dir + "Generated-Particles/"
	if not os.path.exists(gen_point_dir):
		os.makedirs(gen_point_dir)
	gen_image_dir = out_dir + "Generated-Images/"
	if not os.path.exists(gen_image_dir):
		os.makedirs(gen_image_dir)
	gen_embeddings = []
	gen_points_paths = []
	gen_image_paths = []
	# Sample to generate new examples
	for index in range(1, num_samples+1):
		print("Generating " +str(index)+'/'+str(num_samples))
		name = 'Generated_sample_' + pad_index(index)
		# Generate embedding
		sampled_embedding, base_index = PointSampler.sample()
		gen_embeddings.append(sampled_embedding)
		# Generate particles
		gen_points = PointEmbedder.project(sampled_embedding)
		gen_points_path = gen_point_dir + name + ".particles"
		np.savetxt(gen_points_path, gen_points)
		gen_points_paths.append(gen_points_path)
		# Generate image
		base_image_path = orig_img_list[base_index]
		base_particles_path = orig_point_list[base_index]
		gen_image_path = GenerateImage(out_dir, gen_points_path, base_image_path, base_particles_path)
		gen_image_paths.append(gen_image_path)
	csv_file = out_dir + "TotalData.csv"
	makeCSV(out_dir + "TotalData.csv", orig_img_list, orig_point_list, embedded_matrix, gen_image_paths, gen_points_paths, gen_embeddings)
	return

############################ Augmentaiton Pipeline Helper Methods ##################################

'''
Reads data from files in given list and turns into one np matrix
'''
def create_data_matrix(file_list):
	data_matrix = []
	for file in file_list:
		data_matrix.append(np.loadtxt(file))
	return np.array(data_matrix)

'''
Pad index 
'''
def pad_index(index):
	name = str(index)
	while len(name) < 4:
		name = "0" + name
	return name

'''
Makes csv of real and augmented data with format:
	image path, particles path, PCA scores
'''
def makeCSV(filename, orig_imgs, orig_points, orig_embeddings, gen_imgs, gen_points, gen_embeddings):
	csv_out = open(filename, "w+")
	# write originals
	for orig_index in range(len(orig_imgs)):
		string = orig_imgs[orig_index] + "," + orig_points[orig_index]
		for score in orig_embeddings[orig_index]:
			string += "," + str(score)
		csv_out.write(string + "\n")
	# write generated
	for gen_index in range(len(gen_imgs)):
		string = gen_imgs[gen_index] + "," + gen_points[gen_index]
		for score in gen_embeddings[gen_index]:
			string += "," + str(score)
		csv_out.write(string + "\n")
	csv_out.close()

'''
Use warp between particles to warp original image into a new image
'''
def GenerateImage(out_dir, gen_particles, base_image, base_particles):
	image_name = gen_particles.split('/')[-1].replace(".particles",".nrrd")
	gen_image = out_dir + "Generated-Images/" + image_name
	cmd = ["shapeworks", 
		"read-image", "--name", base_image,
		"warp-image", "--source", base_particles, "--target", gen_particles, "--stride", "2",
		"write-image", "--name", gen_image]
	subprocess.check_call(cmd)
	return gen_image