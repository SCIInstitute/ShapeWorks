# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example we provide a full pipeline with an example dataset of axis 
aligned ellipsoid meshes.
"""

from zipfile import ZipFile
import os
import sys
import csv
import argparse

from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *


def Run_Pipeline(args):
    datasetName = 'femur-v0'
    filename = datasetName + '.zip'
    parentDir = 'TestFemurMesh/'
    prepDir = parentDir + 'groomed/'
    meshDir = parentDir + datasetName + '/groomed/meshes/'
    
    if not os.path.exists(parentDir):
        os.makedirs(parentDir)
    
    # Check if the data is in the right place
    if not os.path.exists(meshDir):
        print("Missing folder " + meshDir + ". Checking for " + filename)
        if not os.path.exists(filename):
            print("Can't find " + filename)
            import DatasetUtils
            DatasetUtils.downloadDataset(datasetName)
        # extract the zipfile
        print("Extracting data from " + filename + "...")
        with ZipFile(filename, 'r') as zipObj:
            zipObj.extractall(path=parentDir)

    meshFiles = []
    for f in sorted(os.listdir(meshDir)):
        meshFiles.append(meshDir + f)

    if len(meshFiles) == 0:
        print('Zero mesh files found in', meshDir)
        return

    pointDir = parentDir + 'shape_models/'

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
        "debug_projection" : 0,
        "verbosity" : 2,
        "visualizer_enable": 0,
        "visualizer_wireframe": 0,
        # "visualizer_screenshot_directory": "screenshots_" + str(use_case) + "_" + str(num_samples) + "samples_" + str(num_particles) + "particles/",
    }
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, meshFiles, parameterDictionary)

    launchShapeWorksStudio(pointDir, meshFiles, localPointFiles, worldPointFiles)
