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

##import numpy as np
##import matplotlib.pyplot as plt
from zipfile import ZipFile
import os
import csv
import argparse

from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *


"""
Most of the following steps even though wrapped in python functions are using
the underlying c++ code, for which we need to call the source paths to the 
binaries. This step should be common for any use of a function in ShapeWorks.
__This requires the full ShapeWorks to be correctly built/downloaded!__'

These following commands set the temporary environment variables to point to
shapeworks binaries and set the necessary library paths
"""

binpath = "../build/shapeworks-build/binary"
installpath = "../../bin"
os.environ["PATH"] = installpath + ":" + binpath + ":" + os.environ["PATH"]


parser = argparse.ArgumentParser(description='Example ShapeWorks Pipeline')
parser.add_argument("--interactive", help="Run in interactive mode", action="store_true")
parser.add_argument("--start_with_prepped_data", help="Start with already prepped data", action="store_true")
args = parser.parse_args()


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

print("\nStep 1. Extract Data\n")
if args.interactive:
        input("Press Enter to continue")

parentDir="TestEllipsoids/"
filename="Ellipsoids.zip"
if not os.path.exists(parentDir):
	os.makedirs(parentDir)
# extract the zipfile
with ZipFile(filename, 'r') as zipObj:
	zipObj.extractall(path=parentDir)
	if not args.start_with_prepped_data:
		fileList = sorted(glob.glob("TestEllipsoids/Ellipsoids_UnPrepped/*.nrrd"))
	else:
		fileList = sorted(glob.glob("TestEllipsoids/Ellipsoids_Prepped/*.nrrd"))

fileList = fileList[:15]


"""

## GROOM : Data Pre-processing 
For the unprepped data the first few steps are 
-- Isotropic resampling
-- Padding
-- Center of Mass Alignment
-- Rigid Alignment
-- Largets Bounding Box and Cropping 
"""

print("\nStep 2. Groom - Data Pre-processing\n")
if args.interactive:
        input("Press Enter to continue")

parentDir = '../TestEllipsoids/PrepOutput/'
if not os.path.exists(parentDir):
	os.makedirs(parentDir)

if not args.start_with_prepped_data:
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

print("\nStep 3. Groom - Convert to distance transforms\n")
if args.interactive:
        input("Press Enter to continue")

if not args.start_with_prepped_data:
	dtFiles = applyDistanceTransforms(parentDir, croppedFiles)
else:
	dtFiles = applyDistanceTransforms(parentDir, fileList)

"""
## OPTIMIZE : Particle Based Optimization

Now that we have the distance transform representation of data we create 
the parameter files for the shapeworks particle optimization routine.
For more details on the plethora of parameters for shapeworks please refer to
...[link to documentation]

First we need to create a dictionary for all the parameters required by this
optimization routine
"""

print("\nStep 4. Optimize - Particle Based Optimization\n")
if args.interactive:
        input("Press Enter to continue")

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

"""
Now we execute the particle optimization function.
"""
[localPointFiles, worldPointFiles] = runShapeWorksOptimize_Basic(pointDir, dtFiles, parameterDictionary)

"""
## ANALYZE : Shape Analysis and Visualization

The local and world particles will be saved in TestEllipsoids/PointFiles/128
directory, the set of these points on each sahpe constitue a particle based shape model 
or a Point Distribution Model (PDM). This PDM shape representation is 
computationally flexible and efficient and we can use it to perform shape
analysis. Here we provide one of the provided visualization tool in the 
shapeworks codebase : ShapeWorksView2.
This tool will showcase individual shapes with their particle representations,
as well as the PCA model constructed using these point correspondences. The 
PCA modes of variation representing the given shape population can be 
visualized.
"""

print("\nStep 4. Analysis - Launch ShapeWorksView2\n")
if args.interactive:
        input("Press Enter to continue")


launchShapeWorksView2(pointDir, worldPointFiles)

print("\nShapeworks Pipeline Complete!")
