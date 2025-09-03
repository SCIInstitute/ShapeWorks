# -*- coding: utf-8 -*-
"""
==================================================
Example demonstrating Contour Domain in ShapeWorks
==================================================
"""
import os
import glob
import shapeworks as sw
import subprocess
import numpy as np
import zipfile

from ShapeCohortGen.CohortGenerator import Supershapes2DCohortGenerator

def Run_Pipeline(args):
    """
    Download supershapes data. Refer to `generate_supershapes` in this file to see the generation
    process
    """
    print("\nStep 1. Acquire Data\n")
    dataset_name = "supershapes_1mode"
    output_directory = "Output/supershapes_1mode_contour/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # See the generate_supershapes() function in this file for how the data is generated
    zip_file = 'Data/supershapes_1mode_contour.zip'
    with zipfile.ZipFile(zip_file, 'r') as zip_ref:
        zip_ref.extractall("Output/")
#    sw.download_dataset(dataset_name, output_directory)
    dataset_name = "supershapes2D_1mode"
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
     # Create project spreadsheet
    project_location = output_directory
    if not os.path.exists(project_location):
        os.makedirs(project_location)
    # Set subjects
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
    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    project.set_original_domain_types([sw.DomainType.ContourDomain])
    project.set_groomed_domain_types([sw.DomainType.ContourDomain])
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles" : 64,
        "use_normals": 0,
        "normals_strength": 0.0,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 100,
        "optimization_iterations" : 500,
        "starting_regularization" : 100,
        "ending_regularization" : 1,
        "relative_weighting" : 5,
        "initial_relative_weighting" : 0.1,
        "procrustes" : 1,
        "procrustes_interval" : 2,
        "procrustes_scaling" : 1,
        "save_init_splits" : 0,
        "verbosity" : 0,
        "multiscale": 1,
        "multiscale_particles": 8,
      }
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 25
        parameter_dictionary["multiscale_particles"] = 4

    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    project.set_parameters("optimize",parameters)
    spreadsheet_file = project_location + "supershapes_1mode_contour_" + args.option_set+ ".swproj"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --progress --name ' + spreadsheet_file).split()
    subprocess.check_call(optimize_cmd)

    # If tiny test or verify, check results and exit
    sw.utils.check_results(args, spreadsheet_file)

    print("\nStep 4. Analysis - Launch ShapeWorksStudio")
    """
    Step 4: ANALYZE - open in studio
    For more information about the analysis step, see:
    # http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(analyze_cmd)

def generate_supershapes(out_dir):
    m = 6
    n_samples = 25

    g = Supershapes2DCohortGenerator(out_dir)
    filenames = g.generate(n_samples,
                           m=m, n1_degree=5, n2_degree=None, n3_degree=None,
                           default_n=5.0, seed=41)
    return filenames
