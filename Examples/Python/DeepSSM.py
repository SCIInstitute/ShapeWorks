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

print("\nStep 1. Download Data\n")

datasetName = "deepssm_femur"

# # get zipfile if not there
# filename = datasetName + ".zip"
# if not os.path.exists(filename):
# 	print("Can't find " + filename + " in the current directory.")
# 	import DatasetUtils
# 	DatasetUtils.downloadDataset(datasetName)

# set parent dir
parentDir="DeepSSMFemur/"
if not os.path.exists(parentDir):
	os.makedirs(parentDir)

# # extract the zipfile  
# print("Extracting data from " + filename + "...")
# with ZipFile(filename, 'r') as zipObj:
# 	zipObj.extractall(path=parentDir)

img_dir = parentDir + datasetName + '/images/'
model_dir = parentDir + datasetName + '/model/'
numpy_dir = parentDir + '/numpy/'
numpyData = getNumpyData(img_dir, model_dir, numpy_dir)






