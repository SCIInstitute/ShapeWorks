# -*- coding: utf-8 -*-

from zipfile import ZipFile
import os
import csv
import argparse

from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *

from GirderConnector import downloadUseCaseData


binpath = "../build/shapeworks-build/binary"
installpath = "../../bin"
os.environ["PATH"] = installpath + ":" + binpath + ":" + os.environ["PATH"]


parser = argparse.ArgumentParser(description='Example ShapeWorks Pipeline')
parser.add_argument("--interactive", help="Run in interactive mode", action="store", default=0)
parser.add_argument("--use_single_scale", help="Single scale or multi scale optimization", action="store", default=0)
args = parser.parse_args()


print("\nStep 1. Extract Data\n")
if int(args.interactive) != 0:
        input("Press Enter to continue")

parentDir="TestSpheres/"
filename="spheredata.zip"
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
    dtFiles = sorted(glob.glob(parentDir + "*.nrrd"))

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

pointDir = parentDir + 'PointFiles/'
if not os.path.exists(pointDir):
	os.makedirs(pointDir)

if int(args.use_single_scale) != 0:
	parameterDictionary = {
		"number_of_particles" : 128,
		"use_normals": 0,
                "normal_weight": 10,
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
                "number_of_levels" : 5,
                "use_normals": 1,
                "normal_weight": 10,
		"checkpointing_interval" : 200,
		"keep_checkpoints" : 0,
		"iterations_per_split" : 1000,
		"optimization_iterations" : 2000,
        "starting_regularization" : 1000,
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

meanDir   = parentDir + 'MeanReconstruction/'
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

meshDir_local   = parentDir + 'MeshFiles-Local/'
if not os.path.exists(meshDir_local):
	os.makedirs(meshDir_local)

"""
Parameter dictionary for ReconstructSurface cmd tool.
"""
parameterDictionary = {
	"number_of_particles" : 128,
	"mean_prefix" : meanDir + 'sphere',
	"out_prefix" : meshDir_local + 'sphere', 
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

meshDir_global   = parentDir + 'MeshFiles-World/'
if not os.path.exists(meshDir_global):
	os.makedirs(meshDir_global)

"""
Parameter dictionary for ReconstructSurface cmd tool.
"""
parameterDictionary = {
	"number_of_particles" : 128,
	"mean_prefix" : meanDir + 'sphere',
	"out_prefix" : meshDir_global + 'sphere',
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

pcaDir   = parentDir + 'PCAModesFiles/'
if not os.path.exists(pcaDir):
	os.makedirs(pcaDir)
    
"""
Parameter dictionary for ReconstructSamplesAlongPCAModes cmd tool.
"""
parameterDictionary = {
	"number_of_particles" : 128,
	"mean_prefix" : meanDir + 'sphere',
	"out_prefix" : pcaDir + 'sphere', 
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
