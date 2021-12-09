# -*- coding: utf-8 -*-
"""
====================================================================
Example ShapeWorks Pipeline for mesh optimization
====================================================================
In this example we provide a full pipeline with an example dataset of axis 
aligned ellipsoid meshes.
"""
import os
import glob
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils
import numpy as np


def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA
    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "ellipsoid_1mode"
    output_directory = "Output/ellipsoid_mesh/"
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
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample))
            mesh_files = [mesh_files[i] for i in sample_idx]

    # If skipping grooming, use the pregroomed meshes from the portal
    if args.skip_grooming:
        print("Skipping grooming.")
        out_directory = output_directory + dataset_name + '/groomed/meshes/'
        indices = []
        if args.tiny_test:
            indices = [0, 1, 2]
        elif args.use_subsample:
            indices = sample_idx
        mesh_files = sw.data.get_file_list(
            out_directory, ending=".vtk", indices=indices)

    # Else groom the meshes for optimization
    else:
        print("\nStep 2. Groom - Data Pre-processing\n")
        """
        Step 2: GROOMING 
        The required grooming steps are: 
        1. Reference selection
        2. Rigid Alignment
        For more information on grooming see docs/workflow/groom.md
        http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
        """

        # Create a directory for groomed output
        groom_dir = output_directory + 'groomed/'
        if not os.path.exists(groom_dir):
            os.makedirs(groom_dir)

        """
        First, we need to loop over the mesh files and load them
        """
        mesh_list = []
        mesh_names = []
        for mesh_file in mesh_files:
            print('Loading: ' + mesh_file)
            # get current shape name
            mesh_names.append(mesh_file.split('/')
                               [-1].replace('.nrrd', ''))
            # load mesh
            mesh = sw.Mesh(mesh_file)
            # append to the shape list
            mesh_list.append(mesh)

        """
        Grooming Step 1: Select a reference
        This step requires loading all of the meshes at once so the shape
        closest to the mean can be found and selected as the reference. 
        """
        ref_index = sw.find_reference_mesh_index(mesh_list)
        # Make a copy of the reference segmentation
        ref_mesh = mesh_list[ref_index].write(groom_dir + 'reference.vtk')
        ref_name = mesh_names[ref_index]
        print("Reference found: " + ref_name)

        """
        Grooming Step 2: Rigid alignment
        This step rigidly aligns each shape to the selected reference. 
        """
        for mesh, name in zip(mesh_list, mesh_names):
            print('Aligning ' + name + ' to ' + ref_name)
            # compute rigid transformation
            rigid_transform = mesh.createTransform(ref_mesh, sw.Mesh.AlignmentType.Rigid)
            # apply rigid transform
            mesh.applyTransform(rigid_transform)
            # apply centering
            translation = np.eye(4)
            translation[0:3,-1] = -ref_center
            mesh.applyTransform(translation)

        # Save groomed meshes
        mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', mesh_list,
                        mesh_names, extension='vtk', compressed=False, verbose=True)


    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization
    Now we can run optimization directly on the meshes.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Make directory to save optimization output
    point_dir = output_directory + 'shape_models/' + args.option_set
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)
    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 128,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 500,
        "optimization_iterations": 500,
        "starting_regularization": 100,
        "ending_regularization": 0.1,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "domain_type": 'mesh',
        "relative_weighting": 10,
        "initial_relative_weighting": 0.01,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0
    }
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["initial_relative_weighting"] = 0.05
        parameter_dictionary["relative_weighting"] = 1
        parameter_dictionary["starting_regularization"] = 1000
        parameter_dictionary["ending_regularization"] = 10
        parameter_dictionary["iterations_per_split"] = 500
        parameter_dictionary["optimization_iterations"] = 100
    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["use_shape_statistics_after"] = 32
    # Execute the optimization function
    [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize(
        point_dir, mesh_files, parameter_dictionary)

    # Prepare analysis XML
    analyze_xml = point_dir + "/ellipsoid_mesh_analyze.xml"
    AnalyzeUtils.create_analyze_xml(analyze_xml, mesh_files, local_point_files, world_point_files)

    # If tiny test or verify, check results and exit
    AnalyzeUtils.check_results(args, world_point_files)

    print("\nStep 3. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    """
    Step 3: ANALYZE - Shape Analysis and Visualization
    Now we launch studio to analyze the resulting shape model.
    For more information about the analysis step, see docs/workflow/analyze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    AnalyzeUtils.launch_shapeworks_studio(analyze_xml)