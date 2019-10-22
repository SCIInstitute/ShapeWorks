# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example, we provide unpre-processed left atrial dataset,
which consists of the LGE scan and its segmentation across some number of subjects.
Since the data set is not pre-processed, it requires all re-processing steps;
resampling, padding, alignment, etc. To run the ShapeWorks particle-based optimization,
the processed segmentation files are converted to the signed distance transform.
This example is set to serve as a test case for users who want to process the raw(gray-sclae)
images as well as their binary segmentation images.
In this example, the raw and segmentation data are sharing the same functions for pre-processing,
such as the same transformation matrices for the center of mass and rigid alignment or same bounding box for cropping.


First import the necessary modules
"""

import numpy as np
import matplotlib.pyplot as plt
from zipfile import ZipFile
import os
import csv
import argparse
import glob

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

binpath = "../build/shapeworks/src/ShapeWorks-build/bin"
parser = argparse.ArgumentParser(description='Example ShapeWorks LA Pipeline')
parser.add_argument("--interactive", help="Run in interactive mode", action="store_true")
parser.add_argument("--start_with_prepped_data", help="Start with already prepped data", action="store_true")
parser.add_argument("--start_with_image_and_segmentation_data", help = "use images and segmentations data for preprocessing", action="store_true")
parser.add_argument("--use_single_scale", help="Single scale or multi scale optimization", action="store_true")
parser.add_argument("shapeworks_path", help="Path to ShapeWorks executables (default: "+binpath+")", nargs='?', type=str, default=binpath)
args = parser.parse_args()
os.environ["PATH"] = args.shapeworks_path + ":" + os.environ["PATH"]

filename="leftatrium.zip"
# Check if the data is in the right place
if not os.path.exists(filename):
    print("Can't find " + filename + " on the local filesystem.")
    print("Downloading " + filename + " from SCIGirder.")
    downloadUseCaseData(filename)
#fixme: ld_library_path should not be necessary (see github issue #230)
os.environ["LD_LIBRARY_PATH"]= binpath + "/../lib:" + binpath + "/../lib64"

if args.start_with_image_and_segmentation_data:
    parentDir="Test_leftatrium/"

    if not os.path.exists(parentDir):
        os.makedirs(parentDir)
    # extract the zipfile
    with ZipFile(filename, 'r') as zipObj:
        zipObj.extractall(path=parentDir)

        fileList_img = sorted(glob.glob(parentDir + "LGE/*.nrrd"))
        fileList_seg = sorted(glob.glob(parentDir +"segmentation_LGE/*.nrrd"))

    if not args.start_with_prepped_data:
        print("\nStep 1. Extract Data\n")
        if args.interactive:
            input("Press Enter to continue")

        """
        ## GROOM : Data Pre-processing
        For the unprepped data the first few steps are
        -- Isotropic resampling
        -- Padding
        -- Center of Mass Alignment
        -- Rigid Alignment
        -- Largets Bounding Box and Cropping
        """

        parentDir = './Test_leftatriumanData/PrepOutput/'

        print("\nStep 2. Groom - Data Pre-processing\n")
        if args.interactive:
            input("Press Enter to continue")


        """
        Apply isotropic resampling

        For detailed explainations of parameters for resampling volumes, go to
        ... link

        the segmentation and images are resampled independently and the result files are saved in two different directories.
        """

        [resampledFiles_segmentations, resampledFiles_images] = applyIsotropicResampling(parentDir, fileList_seg ,fileList_img, 1, processRaw = True)

        """
        Apply padding

        For detailed explainations of parameters for padding volumes, go to
        ... link

        Both the segmentation and raw images are padded.
        """

        [paddedFiles_segmentations,  paddedFiles_images] = applyPadding(parentDir, resampledFiles_segmentations,resampledFiles_images, 10, processRaw = True)


        """
        Apply center of mass alignment

        For detailed explainations of parameters for center of mass(COM) alignment of volumes, go to
        ... link

        This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
        There is parameter that you can change to switch between cases. processRaw = True, processes raw and binary images with shared parameters.
        """
        [comFiles_segmentations, comFiles_images] = applyCOMAlignment( parentDir, paddedFiles_segmentations, paddedFiles_images , processRaw=True)

        """
        Apply rigid alignment

        For detailed explainations of parameters for rigid alignment of volumes, go to
        ... link

        This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
        There is parameter that you can change to switch between cases. processRaw = True, processes raw and binary images with shared parameters.
        processRaw = False, applies the center of mass alignment only on segemnattion data.
        This function uses the same transfrmation matrix for alignment of raw and segmentation files.
        Rigid alignment needs a reference file to align all the input files, FindMedianImage function defines the median file as the reference.
        """
        medianFile = FindMedianImage(comFiles_segmentations)

        [rigidFiles_segmentations, rigidFiles_images] = applyRigidAlignment(parentDir, comFiles_segmentations, comFiles_images , medianFile, processRaw = True)

        """
        Compute largest bounding box and apply cropping
        processRaw = True, processes raw and binary images with shared parameters.
        processRaw = False, applies the center of mass alignment only on segemnattion data.
        The function uses the same bounding box to crop the raw and segemnattion data.
        """
        [croppedFiles_segmentations, croppedFiles_images] = applyCropping(parentDir, rigidFiles_segmentations,  rigidFiles_images, processRaw=True)


    print("\nStep 3. Groom - Convert to distance transforms\n")
    if args.interactive:
        input("Press Enter to continue")

    """
    We convert the scans to distance transforms, this step is common for both the
    prepped as well as unprepped data, just provide correct filenames.
    """
    if not args.start_with_prepped_data:
        dtFiles = applyDistanceTransforms(parentDir, croppedFiles_segmentations)
    else:
        dtFiles = applyDistanceTransforms(parentDir, fileList_seg)
else:

    print("\nStep 1. Extract Data\n")
    if args.interactive:
        input("Press Enter to continue")

    parentDir="Test_leftatrium/"

    if not os.path.exists(parentDir):
        os.makedirs(parentDir)

    # extract the zipfile
    with ZipFile(filename, 'r') as zipObj:
        zipObj.extractall(path=parentDir)

        fileList = sorted(glob.glob(parentDir + "segmentation_LGE/*.nrrd"))


    if not args.start_with_prepped_data:
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

        # create the output directory
        parentDir = './Test_leftatrium/PrepOutput/'
        if not os.path.exists(parentDir):
            os.makedirs(parentDir)



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

        paddedFiles = applyPadding(parentDir, resampledFiles ,None, 10)

        """
        Apply center of mass alignment

        For detailed explainations of parameters for center of mass(COM) alignment of volumes, go to
        ... link

         """
        comFiles = applyCOMAlignment(parentDir, paddedFiles, None)

        """
        Apply rigid alignment

        For detailed explainations of parameters for rigid alignment of volumes, go to
        ... link

        Rigid alignment needs a reference file to align all the input files, FindMedianImage function defines the median file as the reference.
        """
        medianFile = FindMedianImage(comFiles)

        rigidFiles = applyRigidAlignment(parentDir, comFiles, None, medianFile)

        """
        Compute largest bounding box and apply cropping
        """
        croppedFiles = applyCropping(parentDir, rigidFiles, None )


    print("\nStep 3. Groom - Convert to distance transforms\n")
    if args.interactive:
        input("Press Enter to continue")

    """
    We convert the scans to distance transforms, this step is common for both the
    prepped as well as unprepped data, just provide correct filenames.
    """
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

pointDir = './Test_leftatrium/PointFiles/'
if not os.path.exists(pointDir):
    os.makedirs(pointDir)


if args.use_single_scale:
    parameterDictionary = {
        "number_of_particles" : 1024,
        "use_normals": 0,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 4000,
        "optimization_iterations" : 4000,
        "starting_regularization" : 50000,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 1,
        "relative_weighting" : 50,
        "initial_relative_weighting" : 0.1,
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


else:
    parameterDictionary = {
        "starting_particles" : 128,
        "number_of_levels" : 4,
        "use_normals": 1,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 4000,
        "optimization_iterations" : 4000,
        "starting_regularization" : 50000,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 1,
        "relative_weighting" : 50,
        "initial_relative_weighting" : 0.1,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "debug_projection" : 0,
        "mesh_based_attributes" : 0,
        "verbosity" : 3
    }

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
if args.interactive:
    input("Press Enter to continue")


meanDir   = './Test_leftatrium/MeanReconstruction/'
if not os.path.exists(meanDir):
    os.makedirs(meanDir)

"""
Parameter dictionary for ReconstructMeanSurface cmd tool.
"""
parameterDictionary = {
    "number_of_particles" : 1024,
    "out_prefix" : meanDir + 'leftatrium',
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
if args.interactive :
    input("Press Enter to continue")

meshDir_local   = './Test_leftatrium/MeshFiles-Local/'
if not os.path.exists(meshDir_local):
    os.makedirs(meshDir_local)

"""
Parameter dictionary for ReconstructSurface cmd tool.
"""
parameterDictionary = {
    "number_of_particles" : 512,
    "mean_prefix" : meanDir + 'leftatrium',
    "out_prefix" : meshDir_local + 'leftatrium',
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
if args.interactive :
    input("Press Enter to continue")

meshDir_global   = './Test_leftatrium/MeshFiles-World/'
if not os.path.exists(meshDir_global):
    os.makedirs(meshDir_global)

"""
Parameter dictionary for ReconstructSurface cmd tool.
"""
parameterDictionary = {
    "number_of_particles" : 1024,
    "mean_prefix" : meanDir + 'leftatrium',
    "out_prefix" : meshDir_global + 'leftatrium',
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
if args.interactive :
    input("Press Enter to continue")

pcaDir   = './Test_leftatrium/PCAModesFiles/'
if not os.path.exists(pcaDir):
    os.makedirs(pcaDir)

"""
Parameter dictionary for ReconstructSamplesAlongPCAModes cmd tool.
"""
parameterDictionary = {
    "number_of_particles" : 1024,
    "mean_prefix" : meanDir + 'leftatrium',
    "out_prefix" : pcaDir + 'leftatrium',
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
The local and world particles will be saved in Test_leftatrium/PointFiles/<number_of_particles>
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
if args.interactive :
    input("Press Enter to continue")

launchShapeWorksView2(pointDir, worldPointFiles)


print("\nStep 10. Analysis - Launch ShapeWorksView2 - dense correspondence model.\n")
if args.interactive:
    input("Press Enter to continue")

launchShapeWorksView2(meshDir_global, worldDensePointFiles)

print("\nShapeworks Pipeline Complete!")



