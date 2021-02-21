# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks DeepSSM
====================================================================
Jadie Adams
"""
import os
import DataAugmentationUtils
import DeepSSMUtils
import CommonUtils

def Run_Pipeline(args):
	if args.tiny_test:
		print("\nRunning a tiny test.")

	print("\nStep 1. Get Data") #############################################################################################
	'''
	Get femur data
	Femur data is downloaded in a zip folder to Data/
    It gets extracted to Output/deep_ssm
	'''
	datasetName = "femur-v0"
	outputDirectory = "Output/deep_ssm/"
	if not os.path.exists(outputDirectory):
		os.makedirs(outputDirectory)
	CommonUtils.download_and_unzip_dataset(datasetName, outputDirectory)

	input_dir = outputDirectory + datasetName + '/'
	# Get image path list
	img_dir = input_dir + "groomed/images/"
	img_list = []
	for file in os.listdir(img_dir):
		img_list.append(img_dir + file)
	img_list = sorted(img_list)
	# Get particles path list
	model_dir =  input_dir + "shape_models/femur/1024/" 
	world_particle_list = []
	local_particle_list = []
	for file in os.listdir(model_dir):
		if "local" in file:
			local_particle_list.append(model_dir + file)
		if "world" in file:
			world_particle_list.append(model_dir + file)
	local_particle_list = sorted(local_particle_list)
	world_particle_list = sorted(world_particle_list)
	# split into train and test
	train_img_list = img_list[:10]
	train_local_particle_list = local_particle_list[:10]
	train_world_particle_list = world_particle_list[:10]
	
	num_samples = 10
	num_dim = 6
	percent_variability=0
	sampler_type = "kde"
	embedded_dim = DataAugmentationUtils.runDataAugmentation(outputDirectory + "Augmentation/", train_img_list, train_local_particle_list, num_samples, num_dim, percent_variability, sampler_type, mixture_num=0, processes=4, world_point_list=train_world_particle_list)
	aug_data_csv = outputDirectory + "Augmentation/TotalData.csv"
	viz_type = "violin"
	DataAugmentationUtils.visualizeAugmentation(aug_data_csv, viz_type)
