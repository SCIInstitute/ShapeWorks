# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example we provide a full pipeline with an example dataset of axis 
aligned ellipsoid meshes.
"""
import os
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils

def Run_Pipeline(args):
    datasetName = "femur-v0"
    outputDirectory = "Output/Femur_mesh/"
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    CommonUtils.get_data(datasetName, outputDirectory)
   
    groomDir = outputDirectory + 'groomed/'
    if not os.path.exists(groomDir):
        os.makedirs(groomDir)
    meshDir = outputDirectory + datasetName + '/groomed/meshes/'
    meshFiles = []
    for f in sorted(os.listdir(meshDir)):
        meshFiles.append(meshDir + f)

    if len(meshFiles) == 0:
        print('Zero mesh files found in', meshDir)
        return

    pointDir = outputDirectory + 'shape_models/'

    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles": 1024,
        "use_shape_statistics_after": 0,
        "use_normals": 0,
        "normal_weight": 0.0,
        "checkpointing_interval" : 10000,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 4000,
        "optimization_iterations" : 500,
        "starting_regularization" : 10,
        "ending_regularization" : 1,
        "recompute_regularization_interval" : 1,
        "domains_per_shape" : 1,
        "domain_type" : 'mesh',
        "relative_weighting" : 10,
        "initial_relative_weighting" : 1,
        "procrustes_interval" : 1,
        "procrustes_scaling" : 1,
        "save_init_splits" : 0,
        "verbosity" : 2,
        "visualizer_enable": 0,
        "visualizer_wireframe": 0,
        # "visualizer_screenshot_directory": "screenshots_" + str(use_case) + "_" + str(num_samples) + "samples_" + str(num_particles) + "particles/",
    }
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, meshFiles, parameterDictionary)

    launchShapeWorksStudio(pointDir, meshFiles, localPointFiles, worldPointFiles)
