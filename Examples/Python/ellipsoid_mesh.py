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
    print("\nStep 1. Extract Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    datasetName = "ellipsoid-v0"
    filename = datasetName + ".zip"
    # Check if the data is in the right place
    if not os.path.exists(filename):
        print("Can't find " + filename + " in the current directory.")
        import DatasetUtils
        DatasetUtils.downloadDataset(datasetName)

    parentDir = "TestEllipsoidsMesh/"
    if not os.path.exists(parentDir):
        os.makedirs(parentDir)
    # extract the zipfile
    with ZipFile(filename, 'r') as zipObj:
        zipObj.extractall(path=parentDir)
        meshFiles = sorted(glob.glob(parentDir + datasetName + "/meshes/*.ply"))
        imageFiles = sorted(glob.glob(parentDir + datasetName + "/images/*.nrrd"))

    meshFiles = meshFiles[:15]
    imageFiles = imageFiles[:15]
    if args.tiny_test:
        args.use_single_scale = 1
        meshFiles = meshFiles[:2]
        imageFiles = imageFiles[:2]

    pointDir = parentDir + 'PointFiles/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles" : 128,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 500,
        "optimization_iterations" : 500,
        "starting_regularization" : 100,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 1,
        "domain_type" : 'mesh',
        "relative_weighting" : 10,
        "initial_relative_weighting" : 0.01,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 3
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

    # Image files are passed because Studio does not support viewing meshes yet.
    launchShapeWorksStudio(pointDir, imageFiles, localPointFiles, worldPointFiles)
