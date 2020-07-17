# -*- coding: utf-8 -*-

from zipfile import ZipFile
import os
import sys
import csv
import argparse

from OptimizeUtils import *
from AnalyzeUtils import *

def Run_Pipeline(args):

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

    datasetName = "lumps"
    testDirectory = f"Test_{datasetName}/"
    originalDataDirectory = testDirectory + datasetName + "/"
    meshFileDirectory = originalDataDirectory + "meshes/"
    pointFilesDirectory = testDirectory + "PointFiles/"
    zipfilename = datasetName + ".zip"

    if not os.path.exists(testDirectory):
        os.makedirs(testDirectory)
    
    # Check if the unzipped data is present
    if not os.path.exists(originalDataDirectory):
        # check if the zipped data is present
        if not os.path.exists(zipfilename):
            print("Can't find " + zipfilename + " in the current directory.")
            import DatasetUtils
            DatasetUtils.downloadDataset(datasetName)
        print("Unzipping " + zipfilename + " into " + testDirectory)
        with ZipFile(zipfilename, 'r') as zipObj:
            zipObj.extractall(path=testDirectory)


    meshFiles = sorted(os.listdir(meshFileDirectory))
    # need to prepend the directory to each file name so that relative paths work
    meshFiles = [meshFileDirectory + filename for filename in meshFiles]


    if args.tiny_test:
        args.use_single_scale = 1
        meshFiles = meshFiles[:2]


    if not os.path.exists(pointFilesDirectory):
        os.makedirs(pointFilesDirectory)

    parameterDictionary = {
        "number_of_particles": 128,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 100,
        "keep_checkpoints": 0,
        "iterations_per_split": 400,
        "optimization_iterations": 100,
        "starting_regularization": 100,
        "ending_regularization": 0.1,
        # "visualizer_enable": 1,
        # "visualizer_wireframe": 1,
        # "visualizer_screenshot_directory": 'screenshots/',
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
        "domain_type": "mesh",
        "relative_weighting": 10,
        "initial_relative_weighting": 1,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "debug_projection": 0,
        "verbosity": 1
    }

    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["visualizer_enable"] = 0

    if not args.use_single_scale:
        parameterDictionary["use_shape_statistics_after"] = 32


    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointFilesDirectory, meshFiles, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()
    

    launchShapeWorksStudio(pointFilesDirectory, meshFiles, localPointFiles, worldPointFiles)

