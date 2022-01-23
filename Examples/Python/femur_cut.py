# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================
This use case is similar to the femur use case except instead of clipping femurs,
cutting planes are used as an optimization constraint.
The femur data set is comprised of segmented meshes of femurs and corresponding CT
images that are not segmented.
The full images can be carried through every step of grooming.
"""
import os
import glob
import subprocess
import numpy as np
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils

def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA
    We define dataset_name which determines which dataset to download from
    the portal and the directory to save output from the use case in.
    This data is comprised of femur meshes and corresponding hip CT scans.
    """
    dataset_name = "femur-v1"
    output_directory = "Output/femur_cut/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = True
        sw.data.download_subset(args.use_case, dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))[:3]
        image_files = sorted(glob.glob(output_directory +
                            dataset_name + "/images/*.nrrd"))[:3]
    # else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))
        image_files = sorted(glob.glob(output_directory + 
                            dataset_name + "/images/*.nrrd"))

        # Select data if using subsample
        if args.use_subsample:
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample))
            mesh_files = [mesh_files[i] for i in sample_idx]

    # If skipping grooming, use the pregroomed meshes from the portal
    if args.skip_grooming:
        print("Skipping grooming.")
        mesh_directory = output_directory + dataset_name + '/groomed/meshes/'
        indices = []
        if args.tiny_test:
            indices = [0, 1, 2]
        elif args.use_subsample:
            indices = sample_idx
        mesh_files = sw.data.get_file_list(mesh_directory, ending=".vtk", indices=indices)

    # Else groom the meshes for optimization
    else:
        print("\nStep 2. Groom - Data Pre-processing\n")
        """
        Step 2: GROOMING
        The required grooming steps are:
        1. Apply smoothing and remeshing and save groomed meshes
        2. Find reflection tansfrom
        3. Select reference mesh
        4. Find rigid alignment transform
        Option to groom corresponding images (includes applying transforms)
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
            Grooming Step 2: Get reflection transform - We have left and 
            right femurs, so we reflect the non-reference side meshes 
            so that all of the femurs can be aligned.
            """
            reflection = np.eye(4) # Identity
            if ref_side in name:
                print("Finding reflection transform for: " + name)
                reflection[0][0] = -1 # Reflect across X
                mesh.applyTransform(reflection)
            reflections.append(reflection)

        """
        Grooming Step 3: Select a reference
        This step requires loading all of the meshes at once so the shape
        closest to the mean can be found and selected as the reference. 
        """
        ref_index = sw.find_reference_mesh_index(mesh_list)
        # Make a copy of the reference mesh
        ref_mesh = mesh_list[ref_index].copy().write(groom_dir + 'reference.vtk')
        ref_name = names[ref_index]
        print("\nReference found: " + ref_name)

        rigid_transforms = [] # save in case grooming images
        for mesh, name in zip(mesh_list, names):
            """
            Grooming Step 4: Rigid alignment
            This step rigidly aligns each shape to the selected reference. 
            """
            print('Finding alingment transfrom from ' + name + ' to ' + ref_name)
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

    # Make directory to save optimization output
    point_dir = output_directory + 'shape_models/' + args.option_set
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)

    # Create spreadsheet
    subjects = []
    number_domains = 1
    for i in range(len(mesh_list)):
        print(names[i])
        subject = sw.Subject()
        subject.set_number_of_domains(number_domains)
        subject.set_groomed_filenames([os.getcwd() + '/' + groomed_mesh_files[i]])
        transform = [ transforms[i].flatten() ]
        subject.set_groomed_transforms(transform)
        subjects.append(subject)

    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles" : 512,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 1000,
        "optimization_iterations" : 500,
        "starting_regularization" : 100,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 1,
        "relative_weighting" : 10,
        "initial_relative_weighting" : 0.01,
        "procrustes_interval" : 1,
        "procrustes_scaling" : 1,
        "save_init_splits" : 1,
        "debug_projection" : 0,
        "verbosity" : 0,
        "use_statistics_in_init" : 0,
        "adaptivity_mode": 0
    }  

    for key in parameter_dictionary:
        parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    parameters.set("domain_type",sw.Variant('mesh'))
    project.set_parameters("optimze",parameters)
    spreadsheet_file = output_directory + "shape_models/proj_parm.xlsx"
    project.save(spreadsheet_file)

    optimizeCmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    subprocess.check_call(optimizeCmd)

    AnalysisCmd = ('ShapeWorksStudio ' + output_directory + "shape_models/proj_parm.xlsx").split()
    subprocess.check_call(AnalysisCmd)
