# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================
Jadie Adams

The femur data set is comprised of segmented meshes of femurs and corresponding CT images that are not segmented.
The first step in grooming is to turn the meshes into the binary volume format shapeworks expects.
The full mages and segmentations must be carried through every stop of grooming. 
Optimization uses single scale.


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

from DatasetUtils import datasets

def Run_Femur_Pipline(args):

    """
    Unzip the data for this tutorial.

    The data is inside the leftatrium.zip, run the following function to unzip the
    data and create necessary supporting files. The files will be Extracted in a
    newly created Directory TestEllipsoids.
    This data is LGE segmentation of left atrium.
    """
    """
    Extract the zipfile into proper directory and create necessary supporting
    files
    """
    # print("\nStep 1. Extract Data\n")
    # if int(args.interactive) != 0:
    #     input("Press Enter to continue")

    # filename = "femurdata.zip"
    # parentDir="TestFemur/"
    # inputDir = 'TestFemur/femurdata/'

    # if not os.path.exists(parentDir):
    #     os.makedirs(parentDir)

    # # # Check if the data is in the right place
    # if not os.path.exists(filename):
    #     print("Can't find " + filename + " on the local filesystem.")
    #     print("Downloading " + filename + " from SCIGirder.")
    #     datasets.downloadDataset(filename)

    # # extract the zipfile
    # with ZipFile(filename, 'r') as zipObj:
    #     zipObj.extractall(path=parentDir)

    # print("\nStep 2. Groom - Data Pre-processing\n")
    # if args.interactive:
    #     input("Press Enter to continue")

    if not args.start_with_prepped_data:
        """
        ## GROOM : Data Pre-processing
        For the unprepped data the first few steps are
        -- Reflect images and meshes
        -- Turn meshes to volumes
        -- Isotropic resampling
        -- Padding
        -- Center of Mass Alignment
        -- Rigid Alignment
        -- clip segementations
        -- Largest Bounding Box and Cropping
        """

        # Directory where grooming output folders will be added
        parentDir = 'TestFemur/PrepOutput/'
        if not os.path.exists(parentDir):
            os.mkdir(parentDir)
    #             # set name specific variables
    #     img_suffix = "1x_hip"
    #     left_suffix = "L_femur"
    #     right_suffix = "R_femur"
    #     mesh_extension = "ply"
    #     reference_side = "left" # somewhat arbitrary

    #     # Get cutting plane
    #     if not args.interactive:
    #         input_mesh = ''
    #         while not input_mesh:
    #             cp_prefix = input("Type the prefix of the sample you wish to use to select the cutting plane (for example: n01_L) and press enter:\n")
    #             if cp_prefix:
    #                 for file in os.listdir(inputDir):
    #                     if cp_prefix in file and mesh_extension in file:
    #                         input_mesh = inputDir + file
    #             if not input_mesh:
    #                 print("Invalid prefix.")
    #         cutting_plane_points = SelectCuttingPlane(input_mesh)
    #         if cp_prefix[-1] =='R':
    #             reference_side = "Right"
    #         print("Cutting plane points defined: ")
    #         print(cutting_plane_points)
    #         print("Continuing to groom.")



    #     """
    #     Reflect
    #     We have left and right femurs, so we will reflect the image if neccesary
    #     so that we have an image for every mesh
    #     """
    #     [fileList_img, fileList_mesh] = anatomyPairsToSingles(parentDir, inputDir, img_suffix, left_suffix, right_suffix, mesh_extension, reference_side)

    #     """
    #     MeshesToVolumes
    #     Shapeworks requires volumes so we need to convert meshes to binary segmentations and distance transform
    #     """
    #     [fileList_imgL, fileList_segL] = MeshesToVolumes(parentDir, fileList_img, fileList_mesh, img_suffix, left_suffix, mesh_extension)
    #     [fileList_imgR, fileList_segR] = MeshesToVolumes(parentDir, fileList_img, fileList_mesh, img_suffix, right_suffix, mesh_extension)
    #     fileList_img = fileList_imgL + fileList_imgR
    #     fileList_seg = fileList_segL + fileList_segR

    #     """
    #     Apply isotropic resampling
    #     the segmentation and images are resampled independently and the result files are saved in two different directories.
    #     """
    #     resampledFiles_segmentations = applyIsotropicResampling(parentDir + "resampled/segmentations", fileList_seg, recenter=False, isBinary=True)
    #     resampledFiles_images = applyIsotropicResampling(parentDir + "resampled/images", fileList_img, recenter=False, isBinary=False)

    #     """
    #     Apply padding
    #     Both the segmentation and raw images are padded.
    #     """
    #     paddedFiles_segmentations = applyPadding(parentDir + "padded2/segementations/", resampledFiles_segmentations, 10)
    #     paddedFiles_images = applyPadding(parentDir + "padded2/images/", resampledFiles_images, 10)


    #     """
    #     Apply center of mass alignment

    #     This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
    #     There is parameter that you can change to switch between cases. processRaw = True, processes raw and binary images with shared parameters.
    #     """
    #     [comFiles_segmentations, comFiles_images] = applyCOMAlignment( parentDir + "com_aligned", paddedFiles_segmentations, raw=paddedFiles_images)

    #     """
    #     Apply rigid alignment
    #     This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
    #     There is parameter that you can change to switch between cases. processRaw = True, processes raw and binary images with shared parameters.
    #     processRaw = False, applies the center of mass alignment only on segemnattion data.
    #     This function uses the same transfrmation matrix for alignment of raw and segmentation files.
    #     Rigid alignment needs a reference file to align all the input files, FindMedianImage function defines the median file as the reference.
    #     """
    #     medianFile = FindReferenceImage(comFiles_segmentations)

    #     [rigidFiles_segmentations, rigidFiles_images] = applyRigidAlignment(parentDir, comFiles_segmentations, comFiles_images , medianFile, processRaw = True)

        medianFile = '/home/sci/jadie/ShapeWorks/Examples/Python/TestFemur/PrepOutput/com_aligned/segmentations/n02_L_femur.isores.pad.com.nrrd'
        rigidFiles_segmentations = ['/home/sci/jadie/ShapeWorks/Examples/Python/TestFemur/PrepOutput/aligned/segmentations/n02_L_femur.isores.pad.com.aligned.nrrd', '/home/sci/jadie/ShapeWorks/Examples/Python/TestFemur/PrepOutput/aligned/segmentations/n02_R_femur.isores.pad.com.aligned.nrrd']
        rigidFiles_images = ['/home/sci/jadie/ShapeWorks/Examples/Python/TestFemur/PrepOutput/aligned/imagesn02_L_femur_1x_hip.isores.pad.com.aligned.nrrd', '/home/sci/jadie/ShapeWorks/Examples/Python/TestFemur/PrepOutput/aligned/imagesn02_R_femur_1x_hip.isores.pad.com.aligned.nrrd']

        # Define cutting plane on median sample
        if args.interactive:
           input_file = medianFile.replace("com_aligned","aligned").replace(".nrrd", ".aligned.DT.nrrd")
           cutting_plane_points = SelectCuttingPlane(input_file)
        # Fix cutting plane points previously selected
        else:
            # Get COM translation
            COM_folder = parentDir + "com_aligned/segmentations/"
            for file in os.listdir(COM_folder):
                if cp_prefix in file and ".txt" in file:
                    COM_filename = COM_folder + file
            COM_filehandler = open(COM_filename, "r")
            line = COM_filehandler.readlines()[2].replace("translation:","")
            trans = []
            for string in line.split():
                trans.append(float(string))
            trans = np.array(trans)
            COM_filehandler.close()
            # Apply COM translation
            print("Translating cutting plane by: ")
            print(trans)
            new_cutting_plane_points = np.zeros(cutting_plane_points.shape)
            for pt_index in range(cutting_plane_points.shape[0]):
                new_cutting_plane_points[pt_index] = cutting_plane_points[pt_index] - trans
            cutting_plane_points = new_cutting_plane_points
            # Get rigid transformation
            rigid_folder = parentDir + "aligned/transformations/"
            for file in os.listdir(rigid_folder):
                if cp_prefix in file and img_suffix not in file:
                    rigid_filename = rigid_folder + file
            rigid_filehandler = open(rigid_filename, "r")
            matrix = []
            lines = rigid_filehandler.readlines()
            index = 0
            for line in lines:
                matrix.append([])
                for string in line.split():
                    matrix[index].append(float(string))
                index += 1
            matrix = np.array(matrix)
            rigid_filehandler.close()
            print("Transforming cutting plane by: ")
            print(matrix)
            new_cutting_plane_points = np.zeros(cutting_plane_points.shape)
            for pt_index in range(cutting_plane_points.shape[0]):
                pt4D = np.array([1,1,1,1])
                pt4D[:3] = cutting_plane_points[pt_index]
                pt = matrix.dot(pt4D)
                new_cutting_plane_points[pt_index] = pt
            cutting_plane_points = new_cutting_plane_points
            print("Cutting plane points: ")
            print(cutting_plane_points)

        """
        Clip Binary Volumes
        We have femurs of different shaft length so we will clip them all using the defined cutting plane
        """
        clippedFiles_segmentations = ClipBinaryVolumes(parentDir, rigidFiles_segmentations, cutting_plane_points.flatten())

        """
        For detailed explainations of parameters for finding the largest bounding box and cropping, go to
        'https://github.com/SCIInstitute/ShapeWorks/blob/master/Prep/Documentation/ImagePrepTools.pdf'


        Compute largest bounding box and apply cropping
        processRaw = True, processes raw and binary images with shared parameters.
        processRaw = False, applies the center of mass alignment only on segemnattion data.
        The function uses the same bounding box to crop the raw and segemnattion data.

        """
        [croppedFiles_segmentations, croppedFiles_images] = applyCropping(parentDir, clippedFiles_segmentations,  rigidFiles_images, processRaw=True)


        print("\nStep 3. Groom - Convert to distance transforms\n")
        if args.interactive:
            input("Press Enter to continue")

        """
        We convert the scans to distance transforms, this step is common for both the
        prepped as well as unprepped data, just provide correct filenames.
        """

        dtFiles = applyDistanceTransforms(parentDir, croppedFiles_segmentations)

    else:
        dtFiles = applyDistanceTransforms(parentDir, fileList_seg)

        print("\nStep 3. Groom - Convert to distance transforms\n")
        if args.interactive:
            input("Press Enter to continue")
    
    """

    ## OPTIMIZE : Particle Based Optimization

    Now that we have the distance transform representation of data we create
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer to
    'https://github.com/SCIInstitute/ShapeWorks/blob/master/Run/Documentation/ParameterDescription.pdf'

    We provide two different mode of operations for the ShapeWorks particle opimization;
    1- Single Scale model takes fixed number of particles and performs the optimization.
    For more detail about the optimization steps and parameters please refer to
    'https://github.com/SCIInstitute/ShapeWorks/blob/master/Run/Documentation/ScriptUsage.pdf'

    2- Multi scale model optimizes for different number of particles in hierarchical manner.
    For more detail about the optimization steps and parameters please refer to
    'https://github.com/SCIInstitute/ShapeWorks/blob/master/Run/Documentation/ScriptUsage.pdf'

    First we need to create a dictionary for all the parameters required by these
    optimization routines
    """
    print("\nStep 4. Optimize - Particle Based Optimization\n")
    if args.interactive:
        input("Press Enter to continue")

    pointDir = './TestFemur/PointFiles/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    if args.use_single_scale:
        parameterDictionary = {
            "number_of_particles" : 1024, 
            "use_normals": 0,
            "normal_weight": 10.0,
            "checkpointing_interval" : 10,
            "keep_checkpoints" : 1,
            "iterations_per_split" : 4000,
            "optimization_iterations" : 4000,
            "starting_regularization" : 100,
            "ending_regularization" : 0.1,
            "recompute_regularization_interval" : 2,
            "domains_per_shape" : 1,
            "relative_weighting" : 10,
            "initial_relative_weighting" : 1,
            "procrustes_interval" : 1,
            "procrustes_scaling" : 1,
            "save_init_splits" : 1,
            "debug_projection" : 0,
            "verbosity" : 3,
            "use_statistics_in_init" : 0
        }

        """
        Now we execute the particle optimization function.
        """
        [localPointFiles, worldPointFiles] = runShapeWorksOptimize_SingleScale(pointDir, dtFiles, parameterDictionary)

    else:
        parameterDictionary = {
            "starting_particles" : 64,
            "number_of_levels" : 4,
            "use_normals": 0, 
            "normal_weight": 10.0,
            "checkpointing_interval" : 10,
            "keep_checkpoints" : 1,
            "iterations_per_split" : 4000,
            "optimization_iterations" : 4000,
            "starting_regularization" : 100,
            "ending_regularization" : 0.1,
            "recompute_regularization_interval" : 2,
            "domains_per_shape" : 1,
            "relative_weighting" : 10,
            "initial_relative_weighting" : 1,
            "procrustes_interval" : 1, 
            "procrustes_scaling" : 1,
            "save_init_splits" : 1, 
            "debug_projection" : 0,
            "verbosity" : 3,
            "use_statistics_in_init" : 0
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


    meanDir   = './TestFemur/MeanReconstruction/'
    if not os.path.exists(meanDir):
        os.makedirs(meanDir)

    """
    Parameter dictionary for ReconstructMeanSurface cmd tool.
    """
    parameterDictionary = {
        "number_of_particles" : 1024,
        "out_prefix" : meanDir + 'femur',
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

    meshDir_local   = './TestFemur/MeshFiles-Local/'
    if not os.path.exists(meshDir_local):
         os.makedirs(meshDir_local)

    """
    Parameter dictionary for ReconstructSurface cmd tool.
    """
    parameterDictionary = {
        "number_of_particles" : 1024,
        "mean_prefix" : meanDir + 'femur',
        "out_prefix" : meshDir_local + 'femur',
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

    meshDir_global   = './TestFemur/MeshFiles-World/'
    if not os.path.exists(meshDir_global):
        os.makedirs(meshDir_global)

    """
    Parameter dictionary for ReconstructSurface cmd tool.
    """
    parameterDictionary = {
        "number_of_particles" : 1024,
        "mean_prefix" : meanDir + 'femur',
        "out_prefix" : meshDir_global + 'femur',
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

    pcaDir   = './TestFemur/PCAModesFiles/'
    if not os.path.exists(pcaDir):
        os.makedirs(pcaDir)

    """
    Parameter dictionary for ReconstructSamplesAlongPCAModes cmd tool.
    """
    parameterDictionary = {
        "number_of_particles" : 1024,
        "mean_prefix" : meanDir + 'femur',
        "out_prefix" : pcaDir + 'femur',
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
    The local and world particles will be saved in TestLeftAtrium/PointFiles/<number_of_particles>
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

    launchShapeWorksView2(pointDir, dtFiles, localPointFiles, worldPointFiles)
    print(rigidFiles_segmentations)

