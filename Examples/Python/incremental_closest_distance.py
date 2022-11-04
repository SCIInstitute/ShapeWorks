import os
import glob
import math
import shutil
import subprocess
import numpy as np
import shapeworks as sw

def Run_Pipeline(args):
    print("\nStep 1. Extract Data")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "supershapes3D"
    output_directory = "Output/incremental_closest_distance/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        sw.data.download_subset(args.use_case, dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))[:9]
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

    """
    Step 2: Get meshes for base model - Sort meshes based on surface-to-surface distance to the reference mesh
    We sort meshes using surface-to-surface distance so that they are ordered from 
    least unque to most unique, then create batches.
    """
    print("\nStep 2: Get meshes for base model ")
    # Load meshes
    meshes = []
    for mesh_file in mesh_files:
        meshes.append(sw.Mesh(mesh_file))
    # Get distances
    distances = np.zeros(len(meshes))
    ref_index = sw.find_reference_mesh_index(meshes)
    ref_mesh = meshes[ref_index]
    for i in range(len(meshes)):
        distances[i] = np.mean(meshes[i].distance(ref_mesh)[0])
    # Sort
    sorted_indices = np.argsort(distances)
    mesh_files_ordered = np.array(mesh_files)[sorted_indices]

    # Cerate shape model with 75% of least unique shapes
    base_model_percent_size = 0.75
    base_shapes_num = int(np.ceil(base_model_percent_size * len(mesh_files_ordered)))
    base_model_mesh_files = sorted(mesh_files_ordered[0: base_shapes_num].tolist())
    outlier_mesh_files = mesh_files_ordered[base_shapes_num:].tolist()
    
    """
    Step 3: Base optimization
    Next we create a base shape model by optimizing on the initial base model
    For more details on the plethora of parameters for shapeworks please refer 
    to: http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """
    print("\nStep 3: Optimize base model")
    # Create project spreadsheet
    project_location = f'{output_directory}/shape_models/'
    if not os.path.exists(project_location):
        os.makedirs(project_location)
    # Remove particle dir if it already exists
    shape_model_dir = f'{project_location}/incremental_closest_distance_particles/'
    if os.path.exists(shape_model_dir):
        shutil.rmtree(shape_model_dir)
    
    subjects = []
    domains_per_shape = 1
    for mesh_file in base_model_mesh_files:
        subject = sw.Subject()
        subject.set_number_of_domains(domains_per_shape)
        rel_mesh_file = sw.utils.get_relative_paths([os.getcwd() + "/" + mesh_file], project_location)
        subject.set_original_filenames(rel_mesh_file)
        subject.set_groomed_filenames(rel_mesh_file)
        subjects.append(subject)
    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 128,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 300,
        "keep_checkpoints": 0,
        "iterations_per_split": 300,
        "optimization_iterations": 150,
        "starting_regularization": 10,
        "ending_regularization": 1,
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
        "relative_weighting": 1,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0
    }

    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["multiscale"] = 1
        parameter_dictionary["multiscale_particles"] = 32
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 8
        parameter_dictionary["optimization_iterations"] = 1
        parameter_dictionary["starting_regularization"] = 10000
        parameter_dictionary["ending_regularization"] = 1000
    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    parameters.set("domain_type",sw.Variant('mesh'))
    project.set_parameters("optimize", parameters)
    spreadsheet_file = output_directory + "shape_models/incremental_closest_distance.xlsx"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    print(optimize_cmd)
    subprocess.check_call(optimize_cmd)

    """
    Step 4: Incrementally optimize 
    Incrementally add one shape which is closest to the shape distribution of base shape model.
    This is followed by adding the next shape which is closest to the new distribution of updated shape model, and so on.
    The mean particles of the current shape model are used as initalization for the incrementally added shape.
    """
    print("\nStep 4: Incremental optimization with closest shape to current shape distribution")

    # Update parameters for incremental optimization 
    parameter_dictionary["use_landmarks"] = 1 				# For particle initialization
    parameter_dictionary["iterations_per_split"] = 0 		# No initialization iterations
    parameter_dictionary["optimization_iterations"] = 50 	# Fewer optimization iterations
    parameter_dictionary["multiscale"] = 0 					# Single scale
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 8
        parameter_dictionary["optimization_iterations"] = 1
        parameter_dictionary["starting_regularization"] = 10000
        parameter_dictionary["ending_regularization"] = 1000
    current_shape_model_dir = shape_model_dir
    current_mesh_files = base_model_mesh_files
    reconstructed_meshes_dir = f'{project_location}/mesh_reconstruction/'
    local_point_files = sorted(glob.glob(f'{current_shape_model_dir}/*_local.particles'))
    world_point_files = sorted(glob.glob(f'{current_shape_model_dir}/*_world.particles'))
    if not os.path.exists(reconstructed_meshes_dir):
        os.makedirs(reconstructed_meshes_dir)
    incremental_total_runs = len(outlier_mesh_files)
    print(f'Adding {incremental_total_runs} shapes incrementally...')
    for cnt in range(incremental_total_runs):
        # Reconstruct shapes for current shape distribution
        print(f'Incremental Run #{cnt+1}')

        reconstructed_mesh_files = sw.utils.reconstruct_meshes(current_mesh_files, local_point_files, world_point_files, reconstructed_meshes_dir)

        # Find the closest outlier to the current shape model
        # print(f"Finding the mesh which is closest to the distribution of current shape model")
        overall_distances_array = np.zeros(len(outlier_mesh_files))
        for idx in range(len(outlier_mesh_files)):
            cur_distances_array = np.zeros(len(reconstructed_mesh_files))
            cur_excluded_mesh = sw.Mesh(outlier_mesh_files[idx])
            # Find the minimum distance of this excluded mesh from the given base model shape distribution
            for i in range(len(reconstructed_mesh_files)):
                reconst_mesh = sw.Mesh(reconstructed_mesh_files[i])
                cur_distances_array[i] = np.mean(reconst_mesh.distance(cur_excluded_mesh)[0])
            cur_min_distance = np.min(cur_distances_array)
            overall_distances_array[idx] = cur_min_distance

        idx_shape_closest = np.argmin(overall_distances_array)
        closest_shape_name = outlier_mesh_files[idx_shape_closest].split('/')[-1]
        print(f"Adding mesh {closest_shape_name} to current shape model")

        # Add the closest outlier and update the shape model
        updated_model_subjects = []
        for idx in range(len(current_mesh_files)):
            subject = sw.Subject()
            subject.set_number_of_domains(1)
            rel_mesh_file = sw.utils.get_relative_paths([os.getcwd() + "/" + current_mesh_files[idx]], project_location)
            subject.set_original_filenames(rel_mesh_file)            
            subject.set_groomed_filenames(rel_mesh_file)
            rel_particle_file = sw.utils.get_relative_paths([os.getcwd() + "/" + local_point_files[idx]], project_location)
            subject.set_landmarks_filenames(rel_particle_file)
            updated_model_subjects.append(subject)
        
        # Initialize the shape which is incrementally added with mean particles of current shape model
        mean_file_name = 'meanshape.particles'
        sw.utils.findMeanShape(current_shape_model_dir, mean_file_name)
        mean_shape_path = f'{current_shape_model_dir}/{mean_file_name}'
        incremental_subject = sw.Subject()
        incremental_subject.set_number_of_domains(1)
        rel_mesh_file = sw.utils.get_relative_paths([os.getcwd() + "/" + outlier_mesh_files[idx_shape_closest]], project_location)
        incremental_subject.set_original_filenames(rel_mesh_file)
        incremental_subject.set_groomed_filenames(rel_mesh_file)
        rel_particle_file = sw.utils.get_relative_paths([os.getcwd() + "/" + mean_shape_path], project_location)
        incremental_subject.set_landmarks_filenames(rel_particle_file)
        updated_model_subjects.append(incremental_subject)

        # Set updated project
        cur_project = sw.Project()
        cur_project.set_subjects(updated_model_subjects)
        cur_parameters = sw.Parameters()

        # Add param dictionary to spreadsheet
        for key in parameter_dictionary:
            cur_parameters.set(key, sw.Variant([parameter_dictionary[key]]))
        cur_parameters.set("domain_type",sw.Variant('mesh'))
        cur_project.set_parameters("optimize", parameters)
        spreadsheet_file = output_directory + "shape_models/incremental_closest_distance.xlsx"
        cur_project.save(spreadsheet_file)

        # Run optimization for updated model
        optimize_cmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
        subprocess.check_output(optimize_cmd)
        

        # Add the outlier shape which was incrementally added to the current shape model files list
        current_mesh_files.append(outlier_mesh_files[idx_shape_closest])
        local_point_files.append(f'{closest_shape_name.split(".")[0]}_local.particles')
        world_point_files.append(f'{closest_shape_name.split(".")[0]}_world.particles')
        if len(outlier_mesh_files) > 0 :
            outlier_mesh_files.pop(idx_shape_closest)

    
    # If tiny test or verify, check results and exit
    # TODO: Add verification data
    # sw.utils.check_results(args, spreadsheet_file)
    
    """
    Step 5: ANALYZE the final shape model - open in studio
    For more information about the analysis step, see:
    # http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    print("\nStep 5. Analysis - Launch ShapeWorksStudio")
    # analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    # subprocess.check_call(analyze_cmd)

