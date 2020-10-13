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
"""
import os
import glob
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils

def Run_Pipeline(args):
    """
    Data is downloaded in a zip folder to Data/
    It gets extracted to Output/left_atrium
    """
    print("\nStep 1. Extract Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    datasetName = "left_atrium-v0"
    outputDirectory = "Output/left_atrium/"
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    CommonUtils.get_data(datasetName, outputDirectory)

    fileList_img = sorted(glob.glob(outputDirectory + datasetName + "/images/*.nrrd"))
    fileList_seg = sorted(glob.glob(outputDirectory + datasetName + "/segmentations/*.nrrd"))

    if args.tiny_test:
        fileList_img = fileList_img[:5]
        fileList_seg = fileList_seg[:5]
        args.use_single_scale = True
        
    if args.use_subsample:
        sample_idx = sampledata(fileList_seg, int(args.use_subsample))
        fileList_seg= [fileList_seg[i] for i in sample_idx]
        fileList_img = [fileList_img[i] for i in sample_idx]
        
    if args.start_with_prepped_data:
        dtFiles = sorted(glob.glob(outputDirectory + datasetName + "groomed/distance_transforms/*.nrrd"))
        
        if args.use_subsample:
            dtFiles = [dtFiles[i] for i in sample_idx]
        if args.tiny_test:
            dtFiles  = dtFiles[:2]
        
    else:

        groomDir = outputDirectory + 'groomed/'
        if not os.path.exists(groomDir):
            os.makedirs(groomDir)
        
        if args.start_with_image_and_segmentation_data and fileList_img:
            """
            ## GROOM : Data Pre-processing
            For the unprepped data the first few steps are
            -- Isotropic resampling
            -- Center
            -- Padding
            -- Center of Mass Alignment
            -- Rigid Alignment
            -- Largest Bounding Box and Cropping
            """

            print("\nStep 2. Groom - Data Pre-processing\n")
            if args.interactive:
                input("Press Enter to continue")

            """
            Apply isotropic resampling
            the segmentation and images are resampled independently and the result files are saved in two different directories.
            """
            resampledFiles_segmentations = applyIsotropicResampling(groomDir + "resampled/segmentations", fileList_seg, isBinary=True)
            resampledFiles_images = applyIsotropicResampling(groomDir + "resampled/images", fileList_img, isBinary=False)

            """
            Centering
            """
            centeredFiles_segmentations = center(groomDir + "centered/segmentations", resampledFiles_segmentations)
            centeredFiles_images = center(groomDir + "centered/images", resampledFiles_images)

            """
            Apply padding
            Both the segmentation and raw images are padded.
            """
            paddedFiles_segmentations = applyPadding(groomDir + 'padded/segmentations', centeredFiles_segmentations, 10)
            paddedFiles_images = applyPadding(groomDir+ 'padded/images', centeredFiles_images, 10)

            """
            Apply center of mass alignment
            This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
            There is parameter that you can change to switch between cases. processRaw = True, processes raw and binary images with shared parameters.
            """
            [comFiles_segmentations, comFiles_images] = applyCOMAlignment(groomDir + "com_aligned", paddedFiles_segmentations, paddedFiles_images, processRaw=True)

            """
            Apply rigid alignment
            This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
            There is parameter that you can change to switch between cases. processRaw = True, processes raw and binary images with shared parameters.
            processRaw = False, applies the center of mass alignment only on segemnattion data.
            This function uses the same transfrmation matrix for alignment of raw and segmentation files.
            Rigid alignment needs a reference file to align all the input files, FindMedianImage function defines the median file as the reference.
            """
            medianFile = FindReferenceImage(comFiles_segmentations)

            [rigidFiles_segmentations, rigidFiles_images] = applyRigidAlignment(groomDir + "aligned", comFiles_segmentations, comFiles_images, medianFile, processRaw = True)

            """
            Compute largest bounding box and apply cropping
            processRaw = True, processes raw and binary images with shared parameters.
            processRaw = False, applies the center of mass alignment only on segemnattion data.
            The function uses the same bounding box to crop the raw and segemnattion data.
            """
            croppedFiles_segmentations = applyCropping(groomDir + "cropped/segmentations", rigidFiles_segmentations, groomDir + "aligned/segmentations/*.aligned.nrrd")
            croppedFiles_images = applyCropping(groomDir + "cropped/images", rigidFiles_images, groomDir + "aligned/segmentations/*.aligned.nrrd")

            print("\nStep 3. Groom - Convert to distance transforms\n")
            if args.interactive:
                input("Press Enter to continue")

            """
            We convert the scans to distance transforms, this step is common for both the
            prepped as well as unprepped data, just provide correct filenames.
            """
            
            dtFiles = applyDistanceTransforms(groomDir, croppedFiles_segmentations)

        else:
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
            if args.interactive:
                input("Press Enter to continue")

            """
            Apply isotropic resampling
            """
            resampledFiles = applyIsotropicResampling(groomDir + "resampled", fileList_seg)

            """Apply Centering"""
            centeredFiles = center(groomDir + "centered", resampledFiles)

            """
            Apply padding
            """
            paddedFiles = applyPadding(groomDir + "padded", centeredFiles, 10)

            """
            Apply center of mass alignment
            """
            comFiles = applyCOMAlignment(groomDir + "com_aligned", paddedFiles, None)

            """
            Apply rigid alignment
            Rigid alignment needs a reference file to align all the input files, FindMedianImage function defines the median file as the reference.
            """
            medianFile = FindReferenceImage(comFiles)
            rigidFiles = applyRigidAlignment(groomDir + "aligned", comFiles, None, medianFile)

            """
            Compute largest bounding box and apply cropping
            """
            croppedFiles = applyCropping(groomDir + "cropped", rigidFiles, groomDir + "aligned/*.aligned.nrrd")

            print("\nStep 3. Groom - Convert to distance transforms\n")
            if args.interactive:
                input("Press Enter to continue")

            """
            We convert the scans to distance transforms, this step is common for both the
            prepped as well as unprepped data, just provide correct filenames.
            """
            dtFiles = applyDistanceTransforms(groomDir, croppedFiles)

    """
    ## OPTIMIZE : Particle Based Optimization

    Now that we have the distance transform representation of data we create
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer to
    /docs/workflow/optimize.md

    We provide two different mode of operations for the ShapeWorks particle opimization:
    1- Single Scale model takes fixed number of particles and performs the optimization.
    2- Multi scale model optimizes for different number of particles in hierarchical manner.

    First we need to create a dictionary for all the parameters required by these
    optimization routines
    """

    print("\nStep 4. Optimize - Particle Based Optimization\n")
    if args.interactive:
        input("Press Enter to continue")

    pointDir = outputDirectory + 'shape_models/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles": 512,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 4000,
        "optimization_iterations": 4000,
        "starting_regularization": 1000,
        "ending_regularization": 10,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "relative_weighting": 10,
        "domain_type" : 'image',
        "initial_relative_weighting": 0.1,
        "procrustes_interval": 1,
        "procrustes_scaling": 1,
        "save_init_splits": 0,
        "verbosity": 3
    }

    if not args.use_single_scale:
        parameterDictionary["use_shape_statistics_after"] = 128
        
    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25
        parameterDictionary["iterations_per_split"] = 25
            
    """
    Now we execute the particle optimization function.
    """
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, dtFiles, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()

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

    print("\nStep 5. Analysis - Launch ShapeWorksStudio.\n")
    if args.interactive :
        input("Press Enter to continue")

    launchShapeWorksStudio(pointDir, dtFiles, localPointFiles, worldPointFiles)