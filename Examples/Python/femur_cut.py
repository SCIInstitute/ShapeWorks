# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================
This use case demonstrates optimizing on meshes with alignment transforms and cutting planes.
The femur data set is comprised of segmented meshes of femurs and corresponding CT
images that are not segmented.
"""
import os
import json
import glob
import subprocess
import numpy as np
import shapeworks as sw

def Run_Pipeline(args):
    print("\nStep 1. Acquire Data\n")
    """
    Step 1: ACQUIRE DATA
    We define dataset_name which determines which dataset to download from
    the portal and the directory to save output from the use case in.
    This data is comprised of femur meshes and corresponding hip CT scans.
    """
    output_directory = "Output/femur_cut/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "femur_cut_tiny_test"
        args.use_single_scale = True
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "femur"
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))[:3]
        plane_files = sorted(glob.glob(output_directory +
                            dataset_name + "/constraints/*.json"))[:3]
    # else download the entire dataset
    else:
        dataset_name = "femur_cut"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "femur"
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))
        plane_files = sorted(glob.glob(output_directory +
                            dataset_name + "/constraints/*.json"))

        # Select data if using subsample
        if args.use_subsample:
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample))
            mesh_files = [mesh_files[i] for i in sample_idx]
            plane_files = [plane_files[i] for i in sample_idx]

    print("\nStep 2. Groom - Data Pre-processing\n")
    """
    Step 2: GROOMING
    The required grooming steps are:
    1. Apply smoothing and remeshing and save groomed meshes
    2. Apply clipping with planes for finding alignment transform
    3. Find reflection tansfrom
    4. Select reference mesh
    5. Find rigid alignment transform
    For more information on grooming see docs/workflow/groom.md
    http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
    """

    # Create a directory for groomed output
    groom_dir = output_directory + 'groomed/'
    if not os.path.exists(groom_dir):
        os.makedirs(groom_dir)

    """
    To begin grooming, we loop over the files and load the meshes
    """
    names = []
    mesh_list = []
    for mesh_filename in mesh_files:
        print('Loading: ' + mesh_filename)
        # Get shape name
        name = os.path.basename(mesh_filename).replace('.ply', '')
        names.append(name)
        # Get mesh
        mesh = sw.Mesh(mesh_filename)
        mesh_list.append(mesh)
        """
        Grooming Step 1: Smooth and remeshing
        """
        print('Smoothing and remeshing: ' + name)
        mesh.smooth(iterations=10).remesh(numVertices=10000, adaptivity=1.0)

    # Write groomed meshes
    print("Writing groomed meshes.")
    groomed_mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', mesh_list,
                        names, extension='vtk', compressed=False, verbose=True)

    # Set reference side (arbitrary)
    ref_side = "L" # chosen so reflection happens in tiny test 
    reflections = [] 
    center_translations = []
    for mesh, name in zip(mesh_list, names):
        """
        Grooming step 2: Apply clipping for finsing alignment transform
        """
        # Load plane
        for plane_file in plane_files:
            if name in plane_file:
                corresponding_plane_file = plane_file
        with open(corresponding_plane_file) as json_file:
            plane = json.load(json_file)['planes'][0]['points']
        # Clip mesh
        print("Clipping: " + name)
        mesh.clip(plane[0], plane[1], plane[2])
        """
        Grooming Step 3: Get reflection transform - We have left and 
        right femurs, so we reflect the non-reference side meshes 
        so that all of the femurs can be aligned.
        """
        reflection = np.eye(4) # Identity
        if ref_side in name:
            print("Creating reflection transform for: " + name)
            reflection[0][0] = -1 # Reflect across X
            mesh.applyTransform(reflection)
        reflections.append(reflection)

    """
    Grooming Step 4: Select a reference
    This step requires loading all of the meshes at once so the shape
    closest to the mean can be found and selected as the reference. 
    """
    print("Finding reference mesh...")
    ref_index = sw.find_reference_mesh_index(mesh_list)
    # Make a copy of the reference mesh
    ref_mesh = mesh_list[ref_index].copy().write(groom_dir + 'reference.vtk')
    ref_name = names[ref_index]
    print("\nReference found: " + ref_name)

    rigid_transforms = [] # save in case grooming images
    for mesh, name in zip(mesh_list, names):
        """
        Grooming Step 5: Rigid alignment
        This step rigidly aligns each shape to the selected reference. 
        """
        print('Creating alignment transform from ' + name + ' to ' + ref_name)
        # compute rigid transformation
        rigid_transform = mesh.createTransform(ref_mesh, sw.Mesh.AlignmentType.Rigid, 100)
        # apply rigid transform
        rigid_transforms.append(rigid_transform)

    # Combine transforms to pass to optimizer
    transforms = []
    for reflection, rigid_transform in zip(reflections, rigid_transforms):
        transforms.append(np.matmul(rigid_transform, reflection))


    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization

    Now that we have the distance transform representation of data we create 
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Create project spreadsheet
    project_location = output_directory
    if not os.path.exists(project_location):
        os.makedirs(project_location)
    # Set subjects
    subjects = []
    number_domains = 1
    for i in range(len(mesh_list)):
        subject = sw.Subject()
        subject.set_number_of_domains(number_domains)
        rel_mesh_files = sw.utils.get_relative_paths([os.getcwd() + '/' + mesh_files[i]], project_location)
        subject.set_original_filenames(rel_mesh_files)
        rel_groom_files = sw.utils.get_relative_paths([os.getcwd() + '/' + groomed_mesh_files[i]], project_location)
        subject.set_groomed_filenames(rel_groom_files)
        transform = [ transforms[i].flatten() ]
        subject.set_groomed_transforms(transform)
        rel_plane_files = sw.utils.get_relative_paths([os.getcwd() + '/' + plane_files[i]], project_location)
        subject.set_constraints_filenames(rel_plane_files)
        subjects.append(subject)
    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles" : 512,
        "use_normals": 0,
        "normals_strength": 10.0,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 1000,
        "optimization_iterations" : 500,
        "starting_regularization" : 100,
        "ending_regularization" : 0.1,
        "relative_weighting" : 10,
        "initial_relative_weighting" : 0.1,
        "procrustes" : 1,
        "procrustes_interval" : 1,
        "procrustes_scaling" : 1,
        "save_init_splits" : 1,
        "verbosity" : 0,
    } 
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 25
        parameter_dictionary["iterations_per_split"] = 25
    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["multiscale"] = 1
        parameter_dictionary["multiscale_particles"] = 64

    for key in parameter_dictionary:
        parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    project.set_parameters("optimize",parameters)
    spreadsheet_file = output_directory + "femur_cut_" + args.option_set+ ".swproj"
    project.save(spreadsheet_file)

    # Run optimization
    optimizeCmd = ('shapeworks optimize --progress --name ' + spreadsheet_file).split()
    subprocess.check_call(optimizeCmd)

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
