# Data Augmentation Pipelines
import os
import numpy as np
import multiprocessing as mtps
import json
from DataAugmentationUtils import Utils
from DataAugmentationUtils import Embedder
from DataAugmentationUtils import Sampler
from shapeworks.utils import sw_message
from shapeworks.utils import sw_progress
from shapeworks.utils import sw_check_abort
import shapeworks as sw

################################# Augmentation Pipelines ###############################################

def point_based_aug(out_dir, orig_img_list, orig_point_list, num_samples, num_dim=0, percent_variability=0.95, sampler_type="kde", mixture_num=0, processes=1, orig_world_point_list=None):
	# Get Embedder
	point_matrix = Utils.create_data_matrix(orig_point_list)
	if orig_world_point_list is not None:
		world_point_matrix = Utils.create_data_matrix(orig_world_point_list)
		world_get_local = [Utils.estimate_homogeneous_similar_transform(
			x=p.reshape((-1,3)).T,
			y=point_matrix[i].reshape((-1,3)).T,
		) for i,p in enumerate(world_point_matrix)]
		world_get_local_info = {
			'world_get_local_list':[
				{
					'homogeneous_transformation': m.tolist(),
					'origi_world_point': orig_world_point_list[i],
					'origi_local_point': orig_point_list[i],
				} for i, m in enumerate(world_get_local)
			],
			'generated_particle__world_get_local_index':[],
		}
		PointEmbedder = Embedder.PCA_Embbeder(world_point_matrix, num_dim, percent_variability)
	else:
		PointEmbedder = Embedder.PCA_Embbeder(point_matrix, num_dim, percent_variability)
	num_dim = PointEmbedder.num_dim
	PointEmbedder.write_PCA(out_dir + "PCA_Particle_Info/", "particles") # write PCA info for DeepSSM testing
	embedded_matrix = PointEmbedder.getEmbeddedMatrix()
	# Get sampler
	if sampler_type == "gaussian":
		PointSampler = Sampler.Gaussian_Sampler()
		PointSampler.fit(embedded_matrix) 
	elif sampler_type == "mixture":
		PointSampler = Sampler.Mixture_Sampler()
		PointSampler.fit(embedded_matrix, mixture_num) 
	elif sampler_type == "kde":
		PointSampler = Sampler.KDE_Sampler()
		PointSampler.fit(embedded_matrix) 
	else:
		sw_message("Error sampler_type unrecognized.")
		sw_message("Gaussian, mixture, and KDE currently supported.")
		return 0
	
	# Initialize output folders and lists
	gen_point_dir = out_dir + "Generated-Particles/"
	if not os.path.exists(gen_point_dir):
		os.makedirs(gen_point_dir)
	gen_image_dir = out_dir + "Generated-Images/"
	if not os.path.exists(gen_image_dir):
		os.makedirs(gen_image_dir)
	get_mesh_dir = out_dir + "Generated-Meshes/"
	if not os.path.exists(get_mesh_dir):
		os.makedirs(get_mesh_dir)
	gen_embeddings = []
	gen_points_paths = []
	gen_image_paths = []
	gen_mesh_paths = []
	if processes != 1:
		generate_image_params_list = []
	# Sample to generate new examples
	for index in range(1, num_samples+1):
		if sw_check_abort():
			sw_message("Aborted")
			return 0
		sw_message("Generating " +str(index)+'/'+str(num_samples))
		sw_progress(index / (num_samples+1))
		name = 'Generated_sample_' + Utils.pad_index(index)
		# Generate embedding
		sampled_embedding, base_index = PointSampler.sample()
		gen_embeddings.append(sampled_embedding)
		# Generate particles
		if orig_world_point_list is not None:
			p = PointEmbedder.project(sampled_embedding)
			i = int(np.nanargmin(np.linalg.norm(p-world_point_matrix, axis=(1,2))))
			# TODO add Randomness in the transformation from world to local particles
			gen_points = (world_get_local[i] @ Utils.get_homogeneous_coordinates(p.reshape((-1,3)).T))[:3,:].T
			world_get_local_info['generated_particle__world_get_local_index'].append(i)
		else:
			gen_points = PointEmbedder.project(sampled_embedding)
		gen_points_path = gen_point_dir + name + ".particles"
		np.savetxt(gen_points_path, gen_points)
		gen_points_paths.append(gen_points_path)
		# Generate mesh
		gen_mesh_path = get_mesh_dir + name + ".vtk"
		sw.utils.reconstruct_mesh(gen_points).write(gen_mesh_path)
		# Generate image
		base_image_path = orig_img_list[base_index]
		base_particles_path = orig_point_list[base_index]
		if processes==1:
			gen_image_path = Utils.generate_image(out_dir, gen_points_path, base_image_path, base_particles_path)
			gen_image_paths.append(gen_image_path)
		else:
			generate_image_params_list.append({
				'out_dir':out_dir,
				'gen_points_path':gen_points_path,
				'base_image_path':base_image_path,
				'base_particles_path':base_particles_path,
			})
	if orig_world_point_list is not None:
		# write world to local transformation information for generated particles
		with open(out_dir + '/world_get_local_info.json', 'w') as f:
			json.dump(world_get_local_info, f)
	if processes!=1:
		with mtps.Pool(processes=processes) as p:
			gen_image_paths = p.map(generate_image, generate_image_params_list)
	csv_file = out_dir + "TotalData.csv"
	Utils.make_CSV(out_dir + "TotalData.csv", orig_img_list, orig_point_list, embedded_matrix, gen_image_paths, gen_points_paths, gen_embeddings)
	return num_dim

def generate_image(param):
	gen_image_path = Utils.generate_image(param['out_dir'], param['gen_points_path'], param['base_image_path'], param['base_particles_path'])
	return gen_image_path
