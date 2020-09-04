# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks DeepSSM
====================================================================
Jadie Adams
"""

from zipfile import ZipFile
import os
import sys
import csv
import argparse
import glob
import re
import numpy as np

import DataAugmentationUtils
import DeepSSMUtils

def Run_Pipeline(args):
	if args.tiny_test:
		print("\nRunning a tiny test.")

	print("\nStep 1. Get Data") #############################################################################################
	'''
	Get femur data
	'''
	# set parent dir
	parent_dir="TestDeepSSM/"
	if not os.path.exists(parent_dir):
		os.makedirs(parent_dir)
	datasetName = "femur"
	input_dir = parent_dir + datasetName + '/'
	# get zipfile if not there
	filename = datasetName + ".zip"
	if not os.path.exists(filename):
		print("Can't find " + filename + " in the current directory.")
		import DatasetUtils
		DatasetUtils.downloadDataset(datasetName)
	# extract the zipfile
	if not os.path.exists(input_dir):
		print("Extracting data from " + filename + "...")
		with ZipFile(filename, 'r') as zipObj:
			zipObj.extractall(path=parent_dir)
	# Get image path list
	img_dir = input_dir + "groomed/images/"
	img_list = []
	for file in os.listdir(img_dir):
		img_list.append(img_dir + file)
	img_list = sorted(img_list)
	# Get particles path list
	model_dir =  input_dir + "shape_models/multi-scale/512/" 
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
	- num_PCA is the number of PCA scores to use
	- aug_type is the augmentation method to use (1 is based on just particles wheras 2 is based on images and particles)
	- sample type is the distribution to use for sampling. Can be Gaussian, mixture, or KDE
	'''
	num_samples = 4960
	num_PCA = 6
	sampler_type = "KDE"
	if args.tiny_test:
		num_samples = 4
		num_PCA = 3
	DataAugmentationUtils.RunDataAugmentation(parent_dir + "augmentation/", train_img_list, train_particle_list, num_samples, num_PCA, sampler_type)
	aug_data_csv = parent_dir + "augmentation/TotalData.csv"
	DataAugmentationUtils.VisualizeAugmentation(aug_data_csv)

	print("\n\n\nStep 3. Reformat Data for Pytorch\n") #######################################################################
	'''
	If down_sample is true, model will trian on images half the original size
	If whiten is true, images and PCA scores will be whitened 
	Hyper-paramter batch_size is for training
		Higher batch size will help speed up training but uses more cuda memory, if you get a memory error try reducing the batch size
	'''
	
	down_sample = False
	batch_size = 1
	loader_dir = parent_dir + 'TorchDataLoaders/'
	DeepSSMUtils.getTrainValLoaders(loader_dir, aug_data_csv, batch_size, down_sample)
	DeepSSMUtils.getTestLoader(loader_dir, test_img_list, test_particle_list, down_sample)


	print("\n\n\nStep 4. Train model.\n") #####################################################################################
	'''
	Set training parameters dict
	val_freq sets how often too test on validation set and log
	for example val_freq=1 is every epoch and val_freq=2 is every other
	'''
	parameters = {"epochs":50, "learning_rate":0.001, "val_freq":1}
	if args.tiny_test:
		parameters = {"epochs":5, "learning_rate":0.001, "val_freq":1}
	model_path = DeepSSMUtils.trainDeepSSM(loader_dir, parameters, parent_dir)


	print("\n\n\nStep 5. Test DeepSSM\n") #####################################################################################
	'''
	Test DeepSSM
	'''
	PCA_scores_path = parent_dir + "augmentation/PCA_Particle_Info/"
	DeepSSMUtils.testDeepSSM(parent_dir + 'test/', model_path, loader_dir, PCA_scores_path, num_PCA)
	print('Predicted particles saved at: ' + parent_dir + 'test/')