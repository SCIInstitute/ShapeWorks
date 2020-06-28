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

    meshFiles = sorted(glob.glob("TestCan/can/*.ply"))

    pointDir = 'TestCan/PointFiles/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "use_normals": 0,
        "normal_weight": 0.0,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 100,
        "optimization_iterations" : 0,
        "starting_regularization" : 100,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 1,
        "domain_type" : 'mesh',
        "relative_weighting" : 1,
        "initial_relative_weighting" : 0.00,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "debug_projection" : 0,
        "verbosity" : 1
        }
    if int(args.use_single_scale) != 0:
        parameterDictionary["number_of_particles"] = 16
        [localPointFiles, worldPointFiles] = runShapeWorksOptimize_SingleScale(pointDir, meshFiles, parameterDictionary)

    else:
        parameterDictionary["starting_particles"] = 32
        parameterDictionary["number_of_levels"] = 3
        [localPointFiles, worldPointFiles] = runShapeWorksOptimize_MultiScale(pointDir, meshFiles, parameterDictionary)

    #launchShapeWorksStudio(pointDir, imageFiles, localPointFiles, worldPointFiles)
