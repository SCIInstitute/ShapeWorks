# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Shared Boundaries in ShapeWorks
====================================================================

Demonstrates how to obtain a shape model for two spheres with a shared boundary. The
shared boundary mesh is extracted, and the contour. Shape statistics for each of these
pieces are then jointly optimized
"""
import os
import shapeworks as sw
import glob
import numpy as np
import subprocess

def Run_Pipeline(args):
    """
    Step 1: ACQUIRE DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    print("\nStep 1. Acquire Data\n")
    output_directory = "Output/peanut_shared_boundary/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "peanut_shared_boundary_tiny_test"
        args.use_single_scale = 1
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "peanut"
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.stl"))[:2]
    # Else download the entire dataset
    else:
        dataset_name = "peanut_shared_boundary"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "peanut"
        mesh_files = sorted(glob.glob(output_directory +
                                     dataset_name + "/meshes/*.stl"))

        if args.use_subsample:
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample),domains_per_shape=2)
            mesh_files = [mesh_files[i] for i in sample_idx]

    print("\nStep 2. Groom - Data Pre-processing\n")
    """
    Step 2: GROOMING 
    The required grooming steps are: 
    1. Remesh 
    2. Extract Shared Boundary Surface
    3. Smooth
    4. Extract Boundary Loop contours
    """

    # Create a directory for groomed output
    groom_dir = output_directory + 'groomed/'
    if not os.path.exists(groom_dir):
        os.makedirs(groom_dir)
    """
    First, we need to loop over the mesh files and load them
    """
    # list of shape segmentations
    mesh_list = []
    # list of shape names (shape files prefixes) to be used for saving outputs
    mesh_names = []
    domain_ids = []
    for mesh_file in mesh_files:
        print('Loading: ' + mesh_file)
        # get current shape name
        mesh_name = mesh_file.split('/')[-1].replace('.stl', '')
        mesh_names.append(mesh_name)
        # get domain identifiers
        domain_ids.append(mesh_name.split(".")[0].split("_")[-1])
        
        # load mesh
        mesh = sw.Mesh(mesh_file)
        # do initial grooming steps
        print("Grooming: " + mesh_name)
        mesh.remeshPercent(percentage=0.99, adaptivity=1.0)
        # append to the mesh list
        mesh_list.append(mesh)

    
    #domain identifiers for all shapes
    domain_ids = np.array(domain_ids)
    #shape index for all shapes in domain left 
    domain1_indx = list(np.where(domain_ids == 'l')[0])
    #shape index for all shapes in domain right
    domain2_indx = list(np.where(domain_ids == 'r')[0])


    # 02. Extract shared boundaries and 03. Smooth the meshes
    extracted_shared_meshes = []
    extracted_meshes = []
    extracted_shared_names = [mesh_names[i].replace("l","s") for i in domain1_indx]
    # distance threshold to consider two surfaces as "shared"
    tol = 1e-1
    num_init_domains = 2 # left and right
    for i in range(len(domain1_indx)):
        mesh_l = mesh_list[domain1_indx[i]]
        mesh_r = mesh_list[domain2_indx[i]]
        print("Extracting shared surface between files: ", mesh_names[i*num_init_domains],"\t",mesh_names[i*num_init_domains+1])
        extracted_l,extracted_r,extracted_s = sw.MeshUtils.sharedBoundaryExtractor(mesh_l,mesh_r,tol)
        #smooth the meshes 
        extracted_l.remeshPercent(percentage=0.99, adaptivity=1.0).smooth()
        extracted_r.remeshPercent(percentage=0.99, adaptivity=1.0).smooth()
        extracted_s.smooth()
        # append the extracted meshes to list
        extracted_meshes.append(extracted_l)
        extracted_meshes.append(extracted_r)
        # append the extracted shared surface mesh to list
        extracted_shared_meshes.append(extracted_s)



    # 04. Extract boundary loop
    contours = []
    contour_names = [mesh_names[i].replace("l","c") for i in domain1_indx]
    for i in range(len(extracted_shared_meshes)):
        print("Extracting boundary loop of shared surface: ", extracted_shared_names[i])
        output_contour = sw.MeshUtils.boundaryLoopExtractor(extracted_shared_meshes[i])
        # add to list
        contours.append(output_contour)

    # Save groomed meshes
    groomed_mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', extracted_meshes, mesh_names, extension='vtk')
    
    
    groomed_shared_mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', extracted_shared_meshes, extracted_shared_names, extension = "vtk")
    
    
    contour_files = sw.utils.save_meshes(groom_dir + 'contours/', contours,contour_names, extension = "vtp")
    
    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization

    Now we can run optimization directly on the meshes.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Create project spreadsheet
    project_location = output_directory 
    if not os.path.exists(project_location):
        os.makedirs(project_location)
    domains_per_shape = 4
    # Set subjects
    subjects = []
    
    for i in range(len(domain1_indx)):
        subject = sw.Subject()
        subject.set_number_of_domains(domains_per_shape)
        rel_mesh_files = []
        rel_groom_files = []
        transform = []
        # Adding left and right spheres
        for d in range(num_init_domains):
            rel_mesh_files += sw.utils.get_relative_paths([os.getcwd() + '/' + mesh_files[i*num_init_domains+d]], project_location)
            rel_groom_files += sw.utils.get_relative_paths([os.getcwd() + '/' + groomed_mesh_files[i*num_init_domains+d]], project_location)
            transform.append(np.eye(4).flatten())
        # Adding extracted shared surface
        rel_mesh_files += sw.utils.get_relative_paths([os.getcwd() + '/' + groomed_shared_mesh_files[i]], project_location)
        rel_groom_files += sw.utils.get_relative_paths([os.getcwd() + '/' + groomed_shared_mesh_files[i]], project_location)
        transform.append(np.eye(4).flatten())
        # Addng extracted contour
        rel_mesh_files += sw.utils.get_relative_paths([os.getcwd() + '/' + contour_files[i]], project_location)
        rel_groom_files += sw.utils.get_relative_paths([os.getcwd() + '/' + contour_files[i]], project_location)
        transform.append(np.eye(4).flatten())

        subject.set_groomed_transforms(transform)
        subject.set_groomed_filenames(rel_groom_files)
        subject.set_original_filenames(rel_mesh_files)
        subjects.append(subject)
    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    num_particles = [128,128,128,128]

    parameter_dictionary = {
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 1000,
        "optimization_iterations" : 2500,
        "starting_regularization" :100,
        "ending_regularization" : 0.01,
        "relative_weighting" : 12, 
        "initial_relative_weighting" : 0.01,
        "procrustes_interval" : 3,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 0,
        "multiscale_particles" : 8,
      }

    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        num_particles = [32,32,32,32]
        parameter_dictionary["optimization_iterations"] = 30
        parameter_dictionary["iterations_per_split"] = 30


    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    parameters.set("number_of_particles" ,sw.Variant(num_particles))
    project.set_parameters("optimize", parameters)
    spreadsheet_file = output_directory + "peanut_shared_boundary_" + args.option_set + ".swproj"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --progress --name ' + spreadsheet_file).split()
    subprocess.check_call(optimize_cmd)

    # If tiny test or verify, check results and exit
    sw.utils.check_results_pattern(args, spreadsheet_file, "l_world")

    print("\nStep 4. Analysis - Launch ShapeWorksStudio")
    """
    Step 4: ANALYZE - open in studio
    For more information about the analysis step, see:
    # http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(analyze_cmd)
