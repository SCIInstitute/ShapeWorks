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
import pickle
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
        mesh_files = sw.data.get_file_list(mesh_directory, ending=".ply", indices=indices)

    # Else groom the meshes for optimization
    else:
        print("\nStep 2. Groom - Data Pre-processing\n")
        """
        Step 2: GROOMING
        The required grooming steps are:
        1. Reflect if neccesary
        2. Apply smoothing and remeshing
        3. Centering
        4. Select reference mesh
        5. Rigidly align mesh to reference
        Option to groom corresponding images (includes applying transforms)
        For more information on grooming see docs/workflow/groom.md
        http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
        """

        # Create a directory for groomed output
        groom_dir = output_directory + 'groomed/'
        if not os.path.exists(groom_dir):
            os.makedirs(groom_dir)

        # Set reference side (arbitrary)
        ref_side = "L" # chosen so reflection happens in tiny test 

        """
        To begin grooming, we loop over the files and load the meshes
        """
        names = []
        mesh_list = []
        reflections = [] # save in case grooming images
        center_translations = [] # save in case grooming images
        for mesh_filename in mesh_files:
            print('\nLoading: ' + mesh_filename)
            # Get shape name
            name = os.path.basename(mesh_filename).replace('.ply', '')
            names.append(name)
            # Get mesh
            mesh = sw.Mesh(mesh_filename)
            mesh_list.append(mesh)
            """
            Grooming Step 1: Get reflection transform - We have left and 
            right femurs, so we reflect the non-reference side meshes 
            so that all of the femurs can be aligned.
            """
            reflection = np.eye(4) # Identity
            if ref_side in name:
                print("Reflecting: " + name)
                reflection[0][0] = -1 # Reflect across X
                mesh.applyTransform(reflection)
            reflections.append(reflection)
            """
            Grooming Step 1: Smooth and remeshing
            """
            print('Smoothing and remeshing: ' + name)
            mesh.smooth(iterations=10).remesh(numVertices=10000, adaptivity=1.0)
            """
            Grooming Step 3: Centering
            """
            print("Centering: " + name)
            translation = np.eye(4) # Identity
            translation[:3,-1] = -mesh.center() # Translate center to (0,0,0)
            mesh.applyTransform(translation)
            center_translations.append(translation)

        """
        Grooming Step 4: Select a reference
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
            Grooming Step 5: Rigid alignment
            This step rigidly aligns each shape to the selected reference. 
            """
            print('\nAligning ' + name + ' to ' + ref_name)
            # compute rigid transformation
            rigid_transform = mesh.createTransform(ref_mesh, sw.Mesh.AlignmentType.Rigid, 100)
            # apply rigid transform
            rigid_transforms.append(rigid_transform)
            mesh.applyTransform(rigid_transform)
           
        # Write groomed meshes
        print("\nWriting groomed meshes.")
        mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', mesh_list,
                            names, extension='vtk', compressed=False, verbose=True)
    
        """
        Groom images
        """
        if args.groom_images:
            # Load corresponding images
            print("\nGrooming images:")
            image_list = []
            for name, reflection, translation in zip(names, reflections, center_translations):
                # Get corresponding image path
                prefix = name.split("_")[0]
                for index in range(len(image_files)):
                    if prefix in image_files[index]:
                        corresponding_image_file = image_files[index]
                        break
                print('\nLoading image: ' + corresponding_image_file)
                image = sw.Image(corresponding_image_file)
                # Apply reflection to image
                print("Reflecting image: " + name)
                image.applyTransform(reflection)
                # Apply centering to image
                print("Centering image: " + name)
                image.setOrigin(image.origin() + translation[:3,-1])
                image_list.append(image)
            # Get reference image
            ref_image = image_list[ref_index].copy()
            ref_image.resample([1,1,1], sw.InterpolationType.Linear).write(groom_dir + 'reference.nrrd')
            # Get bounding box
            bounding_box = sw.MeshUtils.boundingBox(mesh_list)
            for image, name, rigid_transform in zip(image_list, names, rigid_transforms):
                # Align image
                print("\nAligning image: " + name)
                image.applyTransform(rigid_transform,
                                     ref_image.origin(),  ref_image.dims(),
                                     ref_image.spacing(), ref_image.coordsys(),
                                     sw.InterpolationType.Linear, meshTransform=True)
                # Crop image
                print('Cropping image: ' + name)
                try:
                    image.crop(bounding_box)
                except:
                    print(image)
                    input(bounding_box)
            # Write images
            print("\nWriting groomed images.")
            image_files = sw.utils.save_images(groom_dir + 'images/', image_list,
                            names, extension='nrrd', compressed=True, verbose=True)

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

    # Cutting planes
    cutting_planes = []
    cutting_plane_counts = []
    for i in range(len(mesh_files)):
        cutting_planes.append(np.array([[-1,-1,-10], [1,-1,-10], [-1,1,-10]]))
        cutting_plane_counts.append(1)

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles" : 512,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 1,
        "iterations_per_split" : 1000,
        "optimization_iterations" : 500,
        "starting_regularization" : 100,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 1,
        "domain_type" : 'mesh',
        "relative_weighting" : 10,
        "initial_relative_weighting" : 0.01,
        "procrustes_interval" : 1,
        "procrustes_scaling" : 1,
        "save_init_splits" : 1,
        "debug_projection" : 0,
        "verbosity" : 0,
        "use_statistics_in_init" : 0,
        "adaptivity_mode": 0,
        "cutting_plane_counts": cutting_plane_counts,
        "cutting_planes": cutting_planes
    }  

    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 25
        parameter_dictionary["iterations_per_split"] = 25
    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["use_shape_statistics_after"] = 64
    
    # Execute the optimization function on distance transforms
    [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize(
        point_dir, mesh_files, parameter_dictionary)

    # If tiny test or verify, check results and exit
    AnalyzeUtils.check_results(args, world_point_files)

    print("\nStep 4. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    """
    Step 4: ANALYZE - Shape Analysis and Visualization

    Now we launch studio to analyze the resulting shape model.
    For more information about the analysis step, see docs/workflow/analyze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    
    # Prepare analysis XML
    analyze_xml = point_dir + "/femur_cut_analyze.xml"
    AnalyzeUtils.create_analyze_xml(analyze_xml, mesh_files, local_point_files, world_point_files)
    AnalyzeUtils.launch_shapeworks_studio(analyze_xml)
