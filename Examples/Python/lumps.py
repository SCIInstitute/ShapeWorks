# -*- coding: utf-8 -*-
import os
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils

def Run_Pipeline(args):
    """
    Data is downloaded in a zip folder to Data/
    It gets extracted to Output/lumps
    """
    print("\nStep 1. Extract Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    datasetName = "lumps-v0"
    outputDirectory = "Output/lumps/"
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    CommonUtils.get_data(datasetName, outputDirectory)

    meshFileDirectory = outputDirectory + datasetName + '/meshes/'
    meshFiles = sorted(os.listdir(meshFileDirectory))
    # need to prepend the directory to each file name so that relative paths work
    meshFiles = [meshFileDirectory + filename for filename in meshFiles]

    if args.tiny_test:
        args.use_single_scale = 1
        meshFiles = meshFiles[:2]

    shapeModelDirectory = outputDirectory + 'shape_models/'
    if not os.path.exists(shapeModelDirectory):
        os.makedirs(shapeModelDirectory)

    parameterDictionary = {
        "number_of_particles": 512,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 100,
        "keep_checkpoints": 0,
        "iterations_per_split": 2000,
        "optimization_iterations": 500,
        "starting_regularization": 10,
        "ending_regularization": 1,
        # "visualizer_enable": 1,
        # "visualizer_wireframe": 1,
        # "visualizer_screenshot_directory": testDirectory + 'screenshots/',
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
        "domain_type": "mesh",
        "relative_weighting": 10,
        "initial_relative_weighting": 1,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 1
    }

    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["visualizer_enable"] = 0

    if not args.use_single_scale:
        parameterDictionary["use_shape_statistics_after"] = 32


    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(shapeModelDirectory, meshFiles, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()
    

    launchShapeWorksStudio(shapeModelDirectory, meshFiles, localPointFiles, worldPointFiles)