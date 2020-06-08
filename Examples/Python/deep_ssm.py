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

from DataAugmentUtils import *
from DeepSSMUtils import *

def Run_Pipeline(args):
	if args.tiny_test:
		print("\nRunning a tiny test.")

	# set parent dir
	parent_dir="TestDeepSSM/"
	if not os.path.exists(parent_dir):
		os.makedirs(parent_dir)
	datasetName = "femur"
	input_dir = parent_dir + datasetName + '/'

	print("\nStep 1. Get Data")
	# get zipfile if not there
	filename = datasetName + ".zip"
	if not os.path.exists(filename):
		print("Can't find " + filename + " in the current directory.")
		import DatasetUtils
		DatasetUtils.downloadDataset(datasetName)

	# extract the zipfile
	print("Extracting data from " + filename + "...")
	with ZipFile(filename, 'r') as zipObj:
		zipObj.extractall(path=parent_dir)

	# Get lists of paths
	# image path list
	img_dir = input_dir + "groomed/images/"
	img_list = []
	for file in os.listdir(img_dir):
		img_list.append(img_dir + file)
	img_list = sorted(img_list)
	# get particles path list
	model_dir =  input_dir + "model/" 
	particle_list = []
	for file in os.listdir(model_dir):
		if "local" in file:
			particle_list.append(model_dir + file)
	particle_list = sorted(particle_list)

	if args.tiny_test:
		img_list = img_list[:4]
		particle_list = particle_list[:4]

	print("\n\n\nStep 2. Augment data\n")
	# num_samples is how many samples to generate 
	num_samples = 3950
	if args.tiny_test:
		num_samples = 16
	# PCA_var_cutoff dictates how many modes of variation are preserved for points and images (a smaller value will result in fewer PCA modes, setting to 1 will use all of them)
	PCA_var_cutoff = 0.97
	# If doResample is 1 it will resample the image to be half the size for faster training (default is 0)
	# If doPad is 1 it will add padding around the image in case the anatomy lies on the image boundary (default is 0)
	aug_data_csv, PCA_scores_path = dataAugment(parent_dir + "augmentation/", img_list, particle_list, num_samples, PCA_var_cutoff, doResample=0, doPad=0)

	print("\n\n\nStep 3. Reformat Data for Pytorch\n")
	# Hyper-paramter batch_size for training
	# Higher batch size will help speed up training but uses more cuda memory
	# If you get a memory error try reducing the batch size
	batch_size = 1
	loader_dir = parent_dir + 'TorchDataLoaders/'
	getTorchDataLoaders(parent_dir + 'TorchDataLoaders/', aug_data_csv, batch_size)

	print("\n\n\nStep 4. Train model.\n")
	# Training parameters dict
	# val_freq sets how often too test on validation set and log
	# for example val_freq=1 is every epoch and val_freq=2 is every other
	parameters = {"epochs":10, "learning_rate":0.001, "val_freq":1}
	if args.tiny_test:
		parameters = {"epochs":5, "learning_rate":0.001, "val_freq":1}
	model_path = train(loader_dir, parameters, parent_dir)

	print("\n\n\nStep 5. Test DeepSSM\n") 
	# Test DeepSSM
	mr_error, rel_error = test(parent_dir + 'test/', model_path, loader_dir, PCA_scores_path)
	print("Average mean root MSE on test set:")
	print(mr_error)
	print("Average relative error on test set:")
	print(rel_error)
	print('Original and predicted particles saved at: ' + parent_dir + 'test/')