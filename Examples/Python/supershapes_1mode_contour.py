# -*- coding: utf-8 -*-
"""
==================================================
Example demonstrating Contour Domain in ShapeWorks
==================================================
"""
import os
import numpy as np
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils
from ShapeCohortGen.CohortGenerator import Supershapes2DCohortGenerator

def Run_Pipeline(args):
    """
    Download supershapes data. Refer to `generate_supershapes` in this file to see the generation
    process
    """
    print("\nStep 1. Extract Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    datasetName = "supershapes2D_1mode-v0"
    outputDirectory = "Output/supershapes_1mode_contour/"

    # See the generate_supershapes() function in this file for how the data is generated
    CommonUtils.download_and_unzip_dataset(datasetName, outputDirectory)
    contourFiles = sorted(glob.glob(outputDirectory + datasetName + "/contours/*.vtp"))

    pointDir = outputDirectory + 'shape_models/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles" : 64,
        "use_normals": 0,
        "normal_weight": 0.0,
        "checkpointing_interval" : 5000,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 100,
        "optimization_iterations" : 500,
        "starting_regularization" : 100,
        "ending_regularization" : 1,
        "recompute_regularization_interval" : 1,
        "domains_per_shape" : 1,
        "domain_type" : 'contour',
        "relative_weighting" : 5,
        "initial_relative_weighting" : 0.1,
        "procrustes_interval" : 2,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 3,
        "use_shape_statistics_after": 4,
      }

    """
    Now we execute a single scale particle optimization function.
    """
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, contourFiles, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()

    print("\nStep 5. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    if args.interactive != 0:
        input("Press Enter to continue")

    launchShapeWorksStudio(pointDir, [], localPointFiles, worldPointFiles)

def generate_supershapes(out_dir):
    m = 6
    n_samples = 25

    g = Supershapes2DCohortGenerator(out_dir)
    filenames = g.generate(n_samples,
                           m=m, n1_degree=5, n2_degree=None, n3_degree=None,
                           default_n=5.0, seed=41)
    return filenames
