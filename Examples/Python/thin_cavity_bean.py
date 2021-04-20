# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Geodesics Distances in ShapeWorks
====================================================================

In this example we provide a full pipeline with an example dataset of ellipsoids with
a thin cavity translating over the x axis
"""
import os
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils


def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    datasetName = "thin_cavity_bean-v0"
    outputDirectory = "Output/thin_cavity_bean/"
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)

    # Download the dataset
    CommonUtils.download_and_unzip_dataset(datasetName, outputDirectory)
    meshFiles = sorted(glob.glob(outputDirectory + datasetName + "/meshes/*.ply"))
    
    # Select data if using subsample
    if args.use_subsample:
        sample_idx = samplemesh(meshFiles, int(args.num_subsample))
        meshFiles = [meshFiles[i] for i in sample_idx]

    pointDir = outputDirectory + 'shape_models/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles" : 1024,
        "use_normals": 1,
        "normal_weight": 10.0,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 150,
        "optimization_iterations" : 2500,
        "starting_regularization" : 3000,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 1,
        "domains_per_shape" : 1,
        "domain_type" : 'mesh',
        "relative_weighting" : 15,
        "initial_relative_weighting" : 0.01,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 3,
        "geodesics_enabled": 1,
      }

    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25

    if not args.use_single_scale:
        parameterDictionary["use_shape_statistics_after"] = 32


    """
    Now we execute a single scale particle optimization function.
    """
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, meshFiles, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()

    print("\nStep 5. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    if args.interactive != 0:
        input("Press Enter to continue")

    launchShapeWorksStudio(pointDir, meshFiles, localPointFiles, worldPointFiles)
