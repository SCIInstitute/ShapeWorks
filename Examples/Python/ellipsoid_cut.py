# -*- coding: utf-8 -*-
"""
====================================================================
ShapeWorks Optimization with Multiple Cutting Planes
====================================================================
This example demonstrates how to optimize with cutting planes on 
simple ellipsoid data.
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
    dataset_name = "ellipsoid_1mode_aligned"
    output_directory = "Output/ellipsoid_cut/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        sw.data.download_subset(
            args.use_case, dataset_name, output_directory)
        file_list = sorted(glob.glob(output_directory +
                                     dataset_name + "/segmentations/*.nrrd"))[:3]
    # Else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        file_list = sorted(glob.glob(output_directory +
                                     dataset_name + "/segmentations/*.nrrd"))

        # Select representative data if using subsample
        if args.use_subsample:
            inputImages =[sw.Image(filename) for filename in file_list]
            sample_idx = sw.data.sample_images(inputImages, int(args.num_subsample))
            file_list = [file_list[i] for i in sample_idx]

    print("\nStep 2. Groom - Create distance transforms\n")
    """
    Step 2: GROOMING 
    
    This use case makes uses prealigned data so the only grooming step 
    required is to convert the segmentations to distance transforms.

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
    for shape_filename in file_list:
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
    dt_files = sw.utils.save_images(groom_dir + 'distance_transforms/', dt_list,
                                    shape_names, extension='nrrd', compressed=True, verbose=True)

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

    # Define the cutting planes
    cutting_plane_points1 = [[10, 10, 0], [-10, -10, 0], [10, -10, 0]]
    cutting_plane_points2 = [[10, 0, 10], [-10, 0 ,10], [10, 0, -10]]
    cp = [cutting_plane_points1, cutting_plane_points2]
    # Cutting planes
    cutting_planes = []
    cutting_plane_counts = []
    for i in range(len(dt_files)):
        cutting_planes.append(cutting_plane_points1)
        cutting_planes.append(cutting_plane_points2)
        cutting_plane_counts.append(2)

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 32,
        "use_normals": 1,
        "normal_weight": 15.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 3000,
        "optimization_iterations": 3000,
        "starting_regularization": 100,
        "ending_regularization": 10,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "domain_type": 'image',
        "relative_weighting": 15,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 2,
        "adaptivity_mode": 0,
        "cutting_plane_counts": cutting_plane_counts,
        "cutting_planes": cutting_planes
    }
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 16
        parameter_dictionary["optimization_iterations"] = 25
    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["use_shape_statistics_after"] = 16

    # Get data input (meshes if running in mesh mode, else distance transforms)
    parameter_dictionary["domain_type"], input_files = sw.data.get_optimize_input(dt_files, args.mesh_mode)

    # Execute the optimization function on distance transforms
    [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize(
        point_dir, input_files, parameter_dictionary)

    # Prepare analysis XML
    analyze_xml = point_dir + "/ellipsoid_cut_analyze.xml"
    AnalyzeUtils.create_analyze_xml(analyze_xml, input_files, local_point_files, world_point_files)

    # If tiny test or verify, check results and exit
    AnalyzeUtils.check_results(args, world_point_files)

    print("\nStep 4. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    """
    Step 4: ANALYZE - Shape Analysis and Visualization

    Now we launch studio to analyze the resulting shape model.
    For more information about the analysis step, see docs/workflow/analyze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    AnalyzeUtils.launch_shapeworks_studio(analyze_xml)
