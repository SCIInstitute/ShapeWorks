# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks DeepSSM
====================================================================
Jadie Adams

Data: Unsegmented Femur CT scans


First import the necessary modules
"""

from zipfile import ZipFile
import os
import sys
import csv
import argparse
import glob
import re
import numpy as np

from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
from DeepSSMUtils import *

# set parent dir
parent_dir="TestDeepSSM/"
if not os.path.exists(parent_dir):
	os.makedirs(parent_dir)

# print("\nStep 1. Download Data\n")

datasetName = "deepssm_femur"

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

print("\nStep 2. Reformat Data for Pytorch\n")

img_dir = parent_dir + datasetName + '/images/'
model_dir = parent_dir + datasetName + '/model/'
pca_path = parent_dir + datasetName + '/PCA_scores.csv'

train_loader, val_loader, test_loader, test_sample_names = getTorchDataLoaders(img_dir, model_dir, pca_path, parent_dir)
input(test_sample_names)

# #debug
# train_loader = "TestDeepSSM/TorchDataLoaders/train"
# val_loader = "TestDeepSSM/TorchDataLoaders/validation"
# test_loader = "TestDeepSSM/TorchDataLoaders/test"

print("\nStep 3. Define model.\n")
model = DeepSSMNet()

print("\nStep 4. Train model.\n")
model_path = train(model, train_loader, val_loader, parent_dir)

print("\nStep 5. Test DeepSSM\n") 

# Test DeepSSM
avg_error = test(model, test_loader)








