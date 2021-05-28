# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================
This example is set to serve as a test case for new ShapeWorks users, and each
step is explained in the shapeworks including the pre-processing, the 
optimization and, the post ShapeWorks visualization.

First import the necessary modules
"""
import os
import glob
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "ellipsoid_joint_rotation"
    output_directory = "Output/ellipsoid_joint_rotation/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)


    sw.data.download_and_unzip_dataset(dataset_name, output_directory)
    file_list = sorted(glob.glob(output_directory +
                                 dataset_name + "/meshes/*.vtk"))

    # Create a directory for groomed output
    groom_dir = output_directory + 'groomed/'
    if not os.path.exists(groom_dir):
        os.makedirs(groom_dir)

    # # """Apply centering"""
    # centeredFiles = center(groom_dir + "centered/segmentations", file_list)

    # # """ Apply padding"""
    # paddedFiles = applyPadding(groom_dir + "padded/segmentations",centeredFiles,20)

    # dtFiles = applyDistanceTransforms(groom_dir, paddedFiles)

    pointDir = output_directory + 'shape_models/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles" : [512,512],
        "use_normals": [0,0],
        "normal_weight": [1.0,1.0],
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 500,
        "optimization_iterations" : 500,
        "starting_regularization" :100,
        "ending_regularization" : 0.5,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 2,
        "domain_type" : 'mesh',
        "relative_weighting" : 10, #10, # 1 for segmentation images
        "initial_relative_weighting" : 0.1,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 3

      }

    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25

 
    """
    Now we execute a single scale particle optimization function.
    """
    print("CALLING OPTIMIZATION CODE")
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(
        pointDir, file_list, parameterDictionary)
    # runShapeWorksOptimize(pointDir, meshFiles, parameterDictionary)
    domains_per_shape = 2
    launchShapeWorksStudio(pointDir, file_list, localPointFiles, worldPointFiles,domains_per_shape)
