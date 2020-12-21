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
	particle_list = []
	for file in os.listdir(model_dir):
		if "local" in file:
			particle_list.append(model_dir + file)
	particle_list = sorted(particle_list)
	# split into train and test
	train_img_list = img_list[:40]
	train_particle_list = particle_list[:40]
	test_img_list = img_list[40:]
	test_particle_list = particle_list[40:]
	# shorten lists for tiny test 
	if args.tiny_test:
		train_img_list = train_img_list[:4]
		train_particle_list = train_particle_list[:4]
		test_img_list = test_img_list[:3]
		test_particle_list = test_particle_list[:3]

	print("\n\n\nStep 2. Augment data\n") ###################################################################################
	'''
	- num_samples is how many samples to generate 
	- num_dim is the number of PCA scores to use
	- aug_type is the augmentation method to use (1 is based on just particles wheras 2 is based on images and particles)
	- sample type is the distribution to use for sampling. Can be gaussian, mixture, or kde
	'''
	num_samples = 4960
	num_dim = 6
	variability_percent = 0.95
	sampler_type = "kde"
	if args.tiny_test:
		num_samples = 4
		num_dim = 0
		variability_percent = 0.99
	DataAugmentationUtils.runDataAugmentation(outputDirectory + "Augmentation/", train_img_list, train_particle_list, num_samples, num_dim, variability_percent, sampler_type)
	aug_data_csv = outputDirectory + "Augmentation/TotalData.csv"
	DataAugmentationUtils.visualizeAugmentation(aug_data_csv)

	print("\n\n\nStep 3. Reformat Data for Pytorch\n") #######################################################################
	'''
	If down_sample is true, model will train on images half the original size
	If whiten is true, images and PCA scores will be whitened 
	Hyper-parameter batch_size is for training
		Higher batch size will help speed up training but uses more cuda memory, if you get a memory error try reducing the batch size
	'''
	
	down_sample = True
	batch_size = 8
	loader_dir = outputDirectory + 'TorchDataLoaders/'
	DeepSSMUtils.getTrainValLoaders(loader_dir, aug_data_csv, batch_size, down_sample)
	DeepSSMUtils.getTestLoader(loader_dir, test_img_list, down_sample)


	print("\n\n\nStep 4. Train model.\n") #####################################################################################
	'''
	Set training parameters dictionary
	val_freq sets how often to test on validation set and log
	for example val_freq=1 is every epoch and val_freq=2 is every other
	'''
	parameters = {"epochs":50, "learning_rate":0.001, "val_freq":1}
	if args.tiny_test:
		parameters = {"epochs":5, "learning_rate":0.001, "val_freq":1}
	model_path = DeepSSMUtils.trainDeepSSM(loader_dir, parameters, outputDirectory)


	print("\n\n\nStep 5. Predict with DeepSSM\n") #####################################################################################
	'''
	Test DeepSSM
	'''
	PCA_scores_path = outputDirectory + "Augmentation/PCA_Particle_Info/"
	prediction_dir = outputDirectory + 'Results/PredictedParticles/'
	DeepSSMUtils.testDeepSSM(prediction_dir, model_path, loader_dir, PCA_scores_path, num_dim)
	print('Predicted particles saved at: ' + prediction_dir)

	print("\n\n\nStep 6. Analyze results.\n") #####################################################################################
	'''
	Analyze DeepSSM
	'''
	DT_dir = input_dir + "groomed/distance_transforms/"
	out_dir = outputDirectory + "Results/"
	mean_prefix = input_dir + "shape_models/femur/mean/femur"
	avg_distance = DeepSSMUtils.analyzeResults(out_dir, DT_dir, prediction_dir, mean_prefix)
	print("Average surface-to-surface distance from the original to predicted shape = " + str(avg_distance))
