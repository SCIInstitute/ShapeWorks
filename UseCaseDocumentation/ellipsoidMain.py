# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example we provide a full pipeline with an exple dataset of axis 
aligned ellipsoids. We provide two different datasets for two different 
senarios, prepared data consists of the binary images which do not require 
alignment/resampling/cropping as pre-processing and only require conversion to
signed distance transforms before running the ShapeWorks particle based 
optimization. Second is the unprepped data which requires additional 
pre-processing steps before it can be fed into the optimization. 

This example is set to serve as a test case for new ShapeWorks users, and each
step is explained in the shapeworks including the pre-processing, the 
optimization and, the post ShapeWorks visualization.

First import the necessary modules
"""

import numpy as np
import matplotlib.pyplot as plt
from zipfile import ZipFile
import os
import csv
from GroomUtils import *
from OptimizeUtils import *

"""
First we decide which data we want to use for the example, prepped or 
unprepped. Following DATA_FLAG when set to 0 selects the prepped data and when
set to NOT 0 will go with unprepped data. Default is 1.
"""

DATA_FLAG = 1

"""
Unzip the data for this tutorial.

The data is inside the Ellipsoids.zip, run the following function to unzip the 
data and create necessary supporting files. The files will be Extracted in a
newly created Directory TestEllipsoids.
This data both prepped and unprepped are binary images of ellipsoids varying
one of the axes while the other two are kept fixed. 
"""
"""
Extract the zipfile into proper directory and create necessary supporting
files
"""
parentDir="TestEllipsoids/"
filename="Ellipsoids.zip"
if not os.path.exists(parentDir):
	os.makedirs(parentDir)
# extract the zipfile
with ZipFile(filename, 'r') as zipObj:
	zipObj.extractall(path=parentDir)
	if DATA_FLAG:
		fileList = sorted(glob.glob("TestEllipsoids/Ellipsoids_UnPrepped/*.nrrd"))
	else:
		fileList = sorted(glob.glob("TestEllipsoids/Ellipsoids_Prepped/*.nrrd"))


"""
Most of the following steps even though wrapped in python functions are using
the undelying c++ code, for whihc we need to call the source paths to the 
binaries. This step should be common for any use of a function in ShapeWorks.
__This requires the full ShapeWorks to be correctly built/downloaded!__'

These following commands set the temporary envisonment variables to point to
shapeworks binaries and set the necessary library paths
"""

binpath = "../build/shapeworks-build/binary"
installpath = "../install/lib"
os.environ["PATH"] = binpath + ":" + os.environ["PATH"]
os.environ["LD_LIBRARY_PATH"]= installpath + ":" + installpath + "/vtk-5.10"

"""
For the unprepped data the first few steps are 
-- Isotropic resampling
-- Padding
-- Center of Mass Alignment
-- Rigid Alignment
-- Largets Bounding Box and Cropping 
"""
parentDir = '../TestEllipsoids/PrepOutput/'
if not os.path.exists(parentDir):
	os.makedirs(parentDir)

if DATA_FLAG:
	"""
	Apply isotropic resampling
	
	For detailed explainations of parameters for resampling volumes, go to
	... link 
	"""
	resampledFiles = applyIsotropicResampling(parentDir, fileList, 1)
	
	"""
	Apply padding

	For detailed explainations of parameters for padding volumes, go to
	... link
	"""

	paddedFiles = applyPadding(parentDir, resampledFiles, 10)

	"""
	Apply center of mass alignment
	
	For detailed explainations of parameters for center of mass (COM) alignment of volumes, go to
	... link
	"""
	comFiles = applyCOMAlignment(parentDir, paddedFiles, paddedFiles)
	"""Apply rigid alignment"""

	rigidFiles = applyRigidAlignment(parentDir, comFiles, comFiles[0])
	
	"""Compute largest bounding box and apply cropping"""
	croppedFiles = applyCropping(parentDir, rigidFiles, rigidFiles)

"""
We convert the scans to distance transforms, this step is common for both the 
prepped as well as unprepped data, just provide correct filenames.
"""

if DATA_FLAG:
	dtFiles = applyDistanceTransforms(parentDir, croppedFiles)
else:
	dtFiles = applyDistanceTransforms(parentDir, fileList)

"""
################ Particle Based Optimization ################

Now that we have the distance transform representation of data we create 
the parameter files for the shapeworks particle optimization routine.
For more details on the plethora of parameters for shapeworks please refer to
...[link to documentation]

First we need to create a dictionary for all the parameters required by this
optimization routine
"""
pointDir = '../TestEllipsoids/PointFiles/'
if not os.path.exists(pointDir):
	os.makedirs(pointDir)
parameterDictionary = {
	"number_of_particles" : 128,
	"checkpointing_interval" : 200,
	"keep_checkpoints" : 0,
	"iterations_per_split" : 1000,
	"optimization_iterations" : 2000,
	"starting_regularization" : 100,
	"ending_regularization" : 0.1,
	"recompute_regularization_interval" : 2,
	"domains_per_shape" : 1,
	"relative_weighting" : 10,
	"initial_relative_weighting" : 0.01,
	"procrustes_interval" : 0,
	"procrustes_scaling" : 0,
	"save_init_splits" : 0,
	"debug_projection" : 0,
	"mesh_based_attributes" : 0,
	"verbosity" : 3
}