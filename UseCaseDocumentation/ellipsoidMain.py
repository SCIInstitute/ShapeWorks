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

from GirderConnector import downloadUseCaseData


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
parser.add_argument("--interactive", help="Run in interactive mode", action="store", default=0)
parser.add_argument("--start_with_prepped_data", help="Start with already prepped data", action="store", default=0)
parser.add_argument("--use_single_scale", help="Single scale or multi scale optimization", action="store", default=0)
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
if int(args.interactive) != 0:
        input("Press Enter to continue")

parentDir="TestEllipsoids/"
filename="Ellipsoids.zip"
if not os.path.exists(parentDir):
	os.makedirs(parentDir)
	
# Check if the data is in the right place
if not os.path.exists(filename):
    print("Can't find " + filename + " on the local filesystem.")
    print("Downloading " + filename + " from SCIGirder.")
    downloadUseCaseData(filename)
    

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
-- Largest Bounding Box and Cropping 
"""

print("\nStep 2. Groom - Data Pre-processing\n")
if int(args.interactive) != 0:
        input("Press Enter to continue")


parentDir = 'TestEllipsoids/PrepOutput/'
if not os.path.exists(parentDir):
	os.makedirs(parentDir)

if int(args.start_with_prepped_data) == 0:
    """
    Apply isotropic resampling

    For detailed explainations of parameters for resampling volumes, go to
    ... link
    """
    resampledFiles = applyIsotropicResampling(parentDir, fileList, None, 1)

    """
    Apply padding

    For detailed explainations of parameters for padding volumes, go to
    ... link
    """

    paddedFiles = applyPadding(parentDir, resampledFiles, None,  10)

    """
    Apply center of mass alignment

    For detailed explainations of parameters for center of mass (COM) alignment of volumes, go to
    ... link
    """
    comFiles = applyCOMAlignment(parentDir, paddedFiles, None)
    """Apply rigid alignment"""

    rigidFiles = applyRigidAlignment(parentDir, comFiles, None, comFiles[0])

    """Compute largest bounding box and apply cropping"""
    croppedFiles = applyCropping(parentDir, rigidFiles, None)

"""
We convert the scans to distance transforms, this step is common for both the 
prepped as well as unprepped data, just provide correct filenames.
"""

print("\nStep 3. Groom - Convert to distance transforms\n")
if int(args.interactive) != 0:
        input("Press Enter to continue")

if int(args.start_with_prepped_data) == 0:
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
if int(args.interactive) != 0:
        input("Press Enter to continue")

pointDir = 'TestEllipsoids/PointFiles/'
if not os.path.exists(pointDir):
	os.makedirs(pointDir)

if int(args.use_single_scale) != 0:
	parameterDictionary = {
		"number_of_particles" : 128,
		"use_normals": 0,
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
	Now we execute a single scale particle optimization function.
	"""
	[localPointFiles, worldPointFiles] = runShapeWorksOptimize_Basic(pointDir, dtFiles, parameterDictionary)
else:
	parameterDictionary = {
		"starting_particles" : 32,
		"number_of_levels" : 3, 
		"use_normals": 0,
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
	Now we execute a multi-scale particle optimization function.
	"""
	[localPointFiles, worldPointFiles] = runShapeWorksOptimize_MultiScale(pointDir, dtFiles, parameterDictionary)

"""
## ANALYZE : Shape Analysis and Visualization

Shapeworks yields relatively sparse correspondence models that may be inadequate to reconstruct 
thin structures and high curvature regions of the underlying anatomical surfaces. 
However, for many applications, we require a denser correspondence model, for example, 
to construct better surface meshes, make more detailed measurements, or conduct biomechanical 
or other simulations on mesh surfaces. One option for denser modeling is 
to increase the number of particles per shape sample. However, this approach necessarily 
increases the computational overhead, especially when modeling large clinical cohorts.

Here we adopt a template-deformation approach to establish an inter-sample dense surface correspondence, 
given a sparse set of optimized particles. To avoid introducing bias due to the template choice, we developed
an unbiased framework for template mesh construction. The dense template mesh is then constructed 
by triangulating the isosurface of the mean distance transform. This unbiased strategy will preserve 
the topology of the desired anatomy  by taking into account the shape population of interest. 
In order to recover a sample-specific surface mesh, a warping function is constructed using the 
sample-level particle system and the mean/template particle system as control points. 
This warping function is then used to deform the template dense mesh to the sample space.

"""


"""
Reconstruct the dense mean surface given the sparse correspondence model.
"""

print("\nStep 5. Analysis - Reconstruct the dense mean surface given the sparse correspodence model.\n")
if args.interactive != 0:
        input("Press Enter to continue")

meanDir   = './TestEllipsoids/MeanReconstruction/'
if not os.path.exists(meanDir):
	os.makedirs(meanDir)
    
"""
Parameter dictionary for ReconstructMeanSurface cmd tool.
"""
parameterDictionary = {
	"number_of_particles" : 128,
	"out_prefix" : meanDir + 'ellipsoid',
	"do_procrustes" : 0,
	"do_procrustes_scaling" : 0,
	"levelsetValue" : 0.0,
       "targetReduction" : 0.0,
       "featureAngle" : 30,
       "lsSmootherIterations" : 1,
       "meshSmootherIterations" : 1,
       "preserveTopology" : 1,
       "qcFixWinding" : 1,
       "qcDoLaplacianSmoothingBeforeDecimation" : 1,
       "qcDoLaplacianSmoothingAfterDecimation" : 1,
       "qcSmoothingLambda" : 0.5,
       "qcSmoothingIterations" : 3,
       "qcDecimationPercentage" : 0.9,
       "normalAngle" : 90,
       "use_tps_transform" : 0,
       "use_bspline_interpolation" : 0,
       "display" : 0,
       "glyph_radius" : 1
        }

runReconstructMeanSurface(dtFiles, localPointFiles, worldPointFiles, parameterDictionary)

"""
Reconstruct the dense sample-specfic surface in the local coordinate system given the dense mean surface
"""

print("\nStep 6. Analysis - Reconstruct sample-specific dense surface in the local coordinate system.\n")
if args.interactive != 0:
        input("Press Enter to continue")

meshDir_local   = './TestEllipsoids/MeshFiles-Local/'
if not os.path.exists(meshDir_local):
	os.makedirs(meshDir_local)

"""
Parameter dictionary for ReconstructSurface cmd tool.
"""
parameterDictionary = {
	"number_of_particles" : 128,
	"mean_prefix" : meanDir + 'ellipsoid',
	"out_prefix" : meshDir_local + 'ellipsoid', 
	"use_tps_transform" : 0,
	"use_bspline_interpolation" : 0,
	"display" : 0,
	"glyph_radius" : 1
}

localDensePointFiles = runReconstructSurface(localPointFiles, parameterDictionary)


"""
Reconstruct the dense sample-specfic surface in the world coordinate system given the dense mean surface
"""

print("\nStep 7. Analysis - Reconstruct sample-specific dense surface in the world coordinate system.\n")
if args.interactive !=0:
        input("Press Enter to continue")

meshDir_global   = './TestEllipsoids/MeshFiles-World/'
if not os.path.exists(meshDir_global):
	os.makedirs(meshDir_global)

"""
Parameter dictionary for ReconstructSurface cmd tool.
"""
parameterDictionary = {
	"number_of_particles" : 128,
	"mean_prefix" : meanDir + 'ellipsoid',
	"out_prefix" : meshDir_global + 'ellipsoid',
	"use_tps_transform" : 0,
	"use_bspline_interpolation" : 0,
	"display" : 0,
	"glyph_radius" : 1
}

worldDensePointFiles = runReconstructSurface(worldPointFiles, parameterDictionary)

"""
Reconstruct dense meshes along dominant pca modes
"""

print("\nStep 8. Analysis - Reconstruct dense surface for samples along dominant PCA modes.\n")
if args.interactive != 0:
        input("Press Enter to continue")

pcaDir   = './TestEllipsoids/PCAModesFiles/'
if not os.path.exists(pcaDir):
	os.makedirs(pcaDir)
    
"""
Parameter dictionary for ReconstructSamplesAlongPCAModes cmd tool.
"""
parameterDictionary = {
	"number_of_particles" : 128,
	"mean_prefix" : meanDir + 'ellipsoid',
	"out_prefix" : pcaDir + 'ellipsoid', 
        "use_tps_transform" : 0,
	"use_bspline_interpolation" : 0,
	"display" : 0,
	"glyph_radius" : 1,
	"maximum_variance_captured" : 0.95,
	"maximum_std_dev" : 2,
	"number_of_samples_per_mode" : 10
}

runReconstructSamplesAlongPCAModes(worldPointFiles, parameterDictionary)

"""
The local and world particles will be saved in TestEllipsoids/PointFiles/128
directory, the set of these points on each shape constitue a particle based shape model 
or a Point Distribution Model (PDM). This PDM shape representation is 
computationally flexible and efficient and we can use it to perform shape
analysis. Here we provide one of the provided visualization tool in the 
shapeworks codebase : ShapeWorksView2.
This tool will showcase individual shapes with their particle representations,
as well as the PCA model constructed using these point correspondences. The 
PCA modes of variation representing the given shape population can be 
visualized.
"""

print("\nStep 9. Analysis - Launch ShapeWorksView2 - sparse correspondence model.\n")
if args.interactive != 0:
        input("Press Enter to continue")

launchShapeWorksView2(pointDir, dtFiles, localPointFiles, worldPointFiles)

print("\nShapeworks Pipeline Complete!")
