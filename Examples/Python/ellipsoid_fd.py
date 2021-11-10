# -*- coding: utf-8 -*-
"""
=====================================================================
Fixed Domains Example for Statistical Shape Modeling using ShapeWorks 
=====================================================================

In this example we work with a dataset of axis aligned ellipsoids. 
This examples is a use case for fixed domains, i.e. we have an existing 
shape model using some ellipsoids and we want to place correspondences 
on new ellisoids (we are provided fully prepped binary images) according 
to the existing shape model.
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
    dataset_name = "ellipsoid_1mode"
    output_directory = "Output/ellipsoid_fd/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    sw.data.download_and_unzip_dataset(dataset_name, output_directory)

    file_list_dts = sorted(glob.glob(
        output_directory + dataset_name + "/groomed/distance_transforms/*.nrrd"))
    file_list_new_segs = sorted(
        glob.glob(output_directory + dataset_name + "/fd_segmentations/*.nrrd"))

    print("\nStep 2. Groom - Create distance transforms\n")
    """
    Step 2: GROOMING 
    
    The new segmentations are prealigned so the only grooming step 
    required is to convert them to distance transforms.

    For more information on grooming see docs/workflow/groom.md
    http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
    """

    # Create a directory for groomed output
    groom_dir = output_directory + 'groomed/'
    if not os.path.exists(groom_dir):
        os.makedirs(groom_dir)

    """
    Converting segmentations to smooth signed distance transforms.
    The computeDT API needs an iso_value that defines the foreground-background interface, to create 
    a smoother interface we first antialiasing the segmentation then compute the distance transform 
    at the zero-level set. We then need to smooth the DT as it will have some remaining aliasing effect 
    of binarization. 
    So the steps are:
        - Antialias 
        - Compute distance transform
        - Apply smoothing
        - Save the distance transform
    """

    # Define distance transform parameters
    iso_value = 0
    sigma = 1.3
    antialias_iterations = 30
    dt_list = []
    shape_names = []
    # Loop over segs and compute smooth DT
    for shape_filename in file_list_new_segs:
        print('Loading: ' + shape_filename)
        # get current shape name
        shape_name = shape_filename.split('/')[-1].replace('.nrrd', '')
        shape_names.append(shape_name)
        # load segmentation
        shape_seg = sw.Image(shape_filename)
        print('Compute DT for segmentation: ' + shape_name)
        shape_seg.antialias(antialias_iterations).computeDT(
            iso_value).gaussianBlur(sigma)
        dt_list.append(shape_seg)
    # Save distance transforms
    new_dt_files = sw.utils.save_images(groom_dir + 'distance_transforms/', dt_list,
                                        shape_names, extension='nrrd', compressed=True, verbose=True)
    # Get list of original and new distance transforms
    dt_files = file_list_dts + new_dt_files

    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization

    Now that we have the distance transform representation of data we create 
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Make directory to save optimization output
    point_dir = output_directory + 'shape_models/' + args.option_set
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)

    """
    Evaluate the meanshape of the existing shape model and use that to initialize the 
    particles on the new shapes
    """
    shape_model_dir = output_directory + dataset_name + "/shape_models/ellipsoid/128/"
    OptimizeUtils.findMeanShape(shape_model_dir)
    mean_shape_path = shape_model_dir + '/meanshape_local.particles'

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 128,
        "use_normals": 0,
        "normal_weight": 15.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 100,
        "optimization_iterations": 2000,
        "starting_regularization": 100,
        "ending_regularization": 0.1,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "domain_type": 'image',
        "relative_weighting": 15,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0,
        "number_fixed_domains": len(file_list_dts),
        "fixed_domain_model_dir": shape_model_dir,
        "mean_shape_path": mean_shape_path,
    }

    # Execute the optimization function
    [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize_FixedDomains(
        point_dir, dt_files, parameter_dictionary)

    # Prepare analysis XML
    analyze_xml = point_dir + "/ellipsoid_fd_analyze.xml"
    AnalyzeUtils.create_analyze_xml(analyze_xml, dt_files, local_point_files, world_point_files)

    # If tiny test or verify, check results and exit
    AnalyzeUtils.check_results(args, world_point_files)

    """
    Step 4: ANALYZE - Shape Analysis and Visualization

    Now we launch studio to analyze the resulting shape model.
    For more information about the analysis step, see docs/workflow/analyze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    print("\nStep 4. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    AnalyzeUtils.launch_shapeworks_studio(analyze_xml)
