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
parentDir="TestDeepSSM/"
if not os.path.exists(parentDir):
	os.makedirs(parentDir)

print("\nStep 1. Download Data\n")

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
# 	zipObj.extractall(path=parentDir)

print("\nStep 2. Reformat Data for Pytorch\n")

img_dir = parentDir + datasetName + '/images/'
model_dir = parentDir + datasetName + '/model/'
pca_path = parentDir + datasetName + '/PCA_scores.csv'

train_loader, val_loader, test_loader = getTorchDataLoaders(img_dir, model_dir, pca_path, parentDir)

# print("\nStep 3. Train DeepSSM\n") 
# deep_model = train(train_loader, val_loader)

# print("\nStep 4. Test DeepSSM\n") 

# # Test DeepSSM
# test_predictions = test(test_loader)

# error = getError(test_labels, test_predictions)








