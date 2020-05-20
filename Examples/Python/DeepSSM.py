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

# from GroomUtils import *
# from OptimizeUtils import *
# from AnalyzeUtils import *
from DeepSSMUtils import *

# set parent dir
parent_dir="TestDeepSSM/"
if not os.path.exists(parent_dir):
	os.makedirs(parent_dir)
datasetName = "femur"
input_dir = parent_dir + datasetName + '/'

# print("\nStep 1. Get Data\n")
# # get zipfile if not there
# filename = datasetName + ".zip"
# if not os.path.exists(filename):
# 	print("Can't find " + filename + " in the current directory.")
# 	import DatasetUtils
# 	DatasetUtils.downloadDataset(datasetName)

# # extract the zipfile
# print("Extracting data from " + filename + "...")
# with ZipFile(filename, 'r') as zipObj:
# 	zipObj.extractall(path=parent_dir)



print("\nStep 2. Augment data\n")

# get image path list
img_dir = input_dir + "groomed_images/"
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

# Data Augmentation
# How many samples to generate 
num_samples = 10
# PCA_var_cutoff dictates how many modes of variation are preserved for points and images
# a smaller value will result in fewer PCA modes, 1 will use all of them
PCA_var_cutoff = 0.97
img_dir, model_dir, pca_path = dataAugment(img_list, particle_list, parent_dir + "augmentation", num_samples, PCA_var_cutoff)




# print("\nStep 3. Reformat Data for Pytorch\n")
# img_dir = parent_dir  + datasetName + '/images/'
# model_dir = parent_dir + datasetName + '/model/'
# pca_path = parent_dir + datasetName + '/PCA_scores.csv'

# img_dir = 'TestDeepSSM_old/deepssm_femur/images/'
# model_dir = 'TestDeepSSM_old/deepssm_femur/model/'
# pca_path = 'TestDeepSSM_old/deepssm_femur/PCA_scores.csv'
# Hyper-paramter batch_size for training
# Higher batch size will help speed up training but uses more cuda memory
# If you get a memory error try reducing the batch size
# batch_size = 2

# train_loader, val_loader, test_loader, test_sample_names = getTorchDataLoaders(img_dir, model_dir, pca_path, parent_dir, batch_size)

# # #debug
# # train_loader = "TestDeepSSM/TorchDataLoaders/train"
# # val_loader = "TestDeepSSM/TorchDataLoaders/validation"
# # test_loader = "TestDeepSSM/TorchDataLoaders/test"

# print("\nStep 4. Define model.\n")
# model = DeepSSMNet()

# print("\nStep 5. Train model.\n")
# # Training parameters dict
# # val_freq sets how often too test on validation set and log
# # for example val_freq=1 is every epoch and val_freq=2 is every other
# parameters = {"epochs":10, "learning_rate":0.001, "val_freq":1}
# model_path = train(model, train_loader, val_loader, parameters, parent_dir)

# print("\nStep 6. Test DeepSSM\n") 

# # Test DeepSSM
# mr_error, rel_error = test(model, test_loader, test_sample_names)
# print("Average mean root MSE on test set:")
# print(mr_error)
# print("Average relative error on test set:")
# print(rel_error)