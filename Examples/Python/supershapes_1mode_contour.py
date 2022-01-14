# -*- coding: utf-8 -*-
"""
==================================================
Example demonstrating Contour Domain in ShapeWorks
==================================================
"""
import os
import glob
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils
from ShapeCohortGen.CohortGenerator import Supershapes2DCohortGenerator

def Run_Pipeline(args):
    """
    Download supershapes data. Refer to `generate_supershapes` in this file to see the generation
    process
    """
    print("\nStep 1. Extract Data\n")
    dataset_name = "supershapes2D_1mode-v0"
    output_directory = "Output/supershapes_1mode_contour/"

    # See the generate_supershapes() function in this file for how the data is generated
    sw.data.download_and_unzip_dataset(dataset_name, output_directory)
    contour_files = sorted(glob.glob(output_directory + dataset_name + "/contours/*.vtp"))

    point_dir = output_directory + 'shape_models/' + args.option_set
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)

    parameter_dictionary = {
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
        "verbosity" : 0,
        "use_shape_statistics_after": 4,
      }

    """
    Now we execute a single scale particle optimization function.
    """
    [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize(point_dir, contour_files, parameter_dictionary)

    # Prepare analysis XML
    analyze_xml = point_dir + "/supershapes_1mode_contour_analyze.xml"
    AnalyzeUtils.create_analyze_xml(analyze_xml, contour_files, local_point_files, world_point_files)

    # If tiny test or verify, check results and exit
    AnalyzeUtils.check_results(args, world_point_files)

    print("\nStep 5. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    AnalyzeUtils.launch_shapeworks_studio(analyze_xml)

def generate_supershapes(out_dir):
    m = 6
    n_samples = 25

    g = Supershapes2DCohortGenerator(out_dir)
    filenames = g.generate(n_samples,
                           m=m, n1_degree=5, n2_degree=None, n3_degree=None,
                           default_n=5.0, seed=41)
    return filenames
