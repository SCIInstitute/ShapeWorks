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
import subprocess
import numpy as np
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
    
    """
    We loop over the files and load the contour
    """
    names = []
    contour_list = []
    for contour_name in contour_files:
        print('Loading: ' + contour_name)
        
        # Get contour
        contour = sw.Mesh(contour_name)
        contour_list.append(contour)

    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization
    """
    point_dir = output_directory + 'shape_models/' + args.option_set
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)

    # Create spreadsheet
    project_location = output_directory + "shape_models/"
    subjects = []
    number_domains = 1
    for i in range(len(contour_files)):
        subject = sw.Subject()
        subject.set_number_of_domains(number_domains)
        rel_files = sw.utils.get_relative_paths([os.getcwd() + '/' + contour_files[i]], project_location)
        subject.set_original_filenames(rel_files)
        #groomed file is same as input file
        subject.set_groomed_filenames(rel_files)
        transform = [ np.eye(4).flatten() ]
        subject.set_groomed_transforms(transform)
        subjects.append(subject)

    project = sw.Project()
    project.set_subjects(subjects)
    project.set_original_domain_types([sw.DomainType.ContourDomain])
    project.set_groomed_domain_types([sw.DomainType.ContourDomain])
    parameters = sw.Parameters()



    parameter_dictionary = {
        "number_of_particles" : 64,
        "use_normals": 0,
        "normals_strength": 0.0,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 100,
        "optimization_iterations" : 500,
        "starting_regularization" : 100,
        "ending_regularization" : 1,
        "recompute_regularization_interval" : 1,
        "domains_per_shape" : 1,
        "relative_weighting" : 5,
        "initial_relative_weighting" : 0.1,
        "procrustes" : 1,
        "procrustes_interval" : 2,
        "procrustes_scaling" : 1,
        "save_init_splits" : 0,
        "verbosity" : 5,
        "multiscale": 1,
        "multiscale_particles": 4,
      }

    """
    Now we execute a single scale particle optimization function.
    """
    for key in parameter_dictionary:
        parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    # parameters.set("domain_type",sw.Variant('contour'))
    project.set_parameters("optimize",parameters)
    spreadsheet_file = output_directory + "shape_models/supershapes_1mode_contour_" + args.option_set+ ".xlsx"
    project.save(spreadsheet_file)

    # Run optimization
    optimizeCmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    subprocess.check_call(optimizeCmd)

    # Analyze - open in studio
    AnalysisCmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(AnalysisCmd)



    # [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize(point_dir, contour_files, parameter_dictionary)

    # # Prepare analysis XML
    # analyze_xml = point_dir + "/supershapes_1mode_contour_analyze.xml"
    # AnalyzeUtils.create_analyze_xml(analyze_xml, contour_files, local_point_files, world_point_files)

    # # If tiny test or verify, check results and exit
    # AnalyzeUtils.check_results(args, world_point_files)

    # print("\nStep 5. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    # AnalyzeUtils.launch_shapeworks_studio(analyze_xml)

def generate_supershapes(out_dir):
    m = 6
    n_samples = 25

    g = Supershapes2DCohortGenerator(out_dir)
    filenames = g.generate(n_samples,
                           m=m, n1_degree=5, n2_degree=None, n3_degree=None,
                           default_n=5.0, seed=41)
    return filenames
