# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Geodesics Distances in ShapeWorks
====================================================================

In this example we provide a full pipeline with an example dataset of ellipsoids with
a thin cavity translating over the x axis
"""
import os
import glob
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils


def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "thin_cavity_bean-v0"
    output_directory = "Output/thin_cavity_bean/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        sw.data.download_subset(args.use_case, dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))[:3]
    # else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))

    # Select data if using subsample
    if args.use_subsample:
        sample_idx = sw.data.samplemeshes(mesh_files, int(args.num_subsample))
        mesh_files = [mesh_files[i] for i in sample_idx]

    # This dataset is prealigned and does not require any grooming steps.

    print("\nStep 2. Optimize with Geodesics\n")
    """
    Step 2: OPTIMIZE 

    Now we can run optimization directly on the meshes using geodesics.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Make directory to save optimization output
    pointDir = output_directory + 'shape_models/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)
    # Create a dictionary for all the parameters required by optimization
    parameterDictionary = {
        "number_of_particles": 1024,
        "use_normals": 1,
        "normal_weight": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 150,
        "optimization_iterations": 2500,
        "starting_regularization": 3000,
        "ending_regularization": 0.1,
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
        "domain_type": 'mesh',
        "relative_weighting": 15,
        "initial_relative_weighting": 0.01,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 3,
        "geodesics_enabled": 1,
    }
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25
    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameterDictionary["use_shape_statistics_after"] = 32
    # Execute the optimization function
    [localPointFiles, worldPointFiles] = OptimizeUtils.runShapeWorksOptimize(
        pointDir, mesh_files, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()

    print("\nStep 3. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    """
    Step 3: ANALYZE - Shape Analysis and Visualization

    Now we launch studio to analyze the resulting shape model.
    For more information about the analysis step, see docs/workflow/analyze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    AnalyzeUtils.launchShapeWorksStudio(
        pointDir, mesh_files, localPointFiles, worldPointFiles)