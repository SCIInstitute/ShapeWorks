# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example we provide a full pipeline with optimizing on femur meshes.
"""
import os
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils

def Run_Pipeline(args):
    """
    If femur.zip is not there it will be downloaded from the ShapeWorks data portal.
    femur.zip will be saved in the /Data folder and the data will be extracted 
    in a newly created directory Output/femur_mesh.
    """
    datasetName = "femur-v0"
    outputDirectory = "Output/femur_mesh/"
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    

    #If tiny_test then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        CommonUtils.download_subset(args.use_case,datasetName, outputDirectory)
        meshFiles = sorted(glob.glob(outputDirectory + datasetName + "/groomed/meshes/*.ply"))[:3]
    #else download the entire dataset
    else:
        CommonUtils.download_and_unzip_dataset(datasetName, outputDirectory)
        meshFiles = sorted(glob.glob(outputDirectory + datasetName + "/groomed/meshes/*.ply"))


    # Select data if using subsample
    if args.use_subsample:
        sample_idx = samplemesh(meshFiles, int(args.num_subsample))
        meshFiles = [meshFiles[i] for i in sample_idx]

    pointDir = outputDirectory + 'shape_models/'

    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles": 512,
        "use_shape_statistics_after": 0,
        "use_normals": 0,
        "normal_weight": 0.0,
        "checkpointing_interval" : 10000,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 500,
        "optimization_iterations" : 500,
        "starting_regularization" : 1000,
        "ending_regularization" : 10,
        "recompute_regularization_interval" : 1,
        "domains_per_shape" : 1,
        "domain_type" : 'mesh',
        "relative_weighting" : 1,
        "initial_relative_weighting" : 0.05,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 2,
        "visualizer_enable": 0,
        "visualizer_wireframe": 0,
        # "visualizer_screenshot_directory": "screenshots_" + str(use_case) + "_" + str(num_samples) + "samples_" + str(num_particles) + "particles/",
    }

    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25
        parameterDictionary["iterations_per_split"] = 25

    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, meshFiles, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()


    launchShapeWorksStudio(pointDir, meshFiles, localPointFiles, worldPointFiles)
