# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks DeepSSM
====================================================================
"""
import os
import pandas as pd
import glob
import shapeworks as sw
import platform
import DataAugmentationUtils
import DeepSSMUtils
import torch
import random
import math
import numpy as np
import subprocess
import itk
import scipy
import json
torch.multiprocessing.set_sharing_strategy('file_system')
random.seed(1)

def Run_Pipeline(args):
    ######################################################################################
    print("\nStep 1. Extract Data")
    """
    Step 1: EXTRACT DATA
    We define dataset_name which determines which dataset to download from
    the portal and the directory to save output from the use case in.
    This data is comprised of femur meshes and corresponding hip CT scans.
    """
    dataset_name = "femur"
    output_directory = os.getcwd() + "/Output/deep_ssm/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    if platform.system() == "Darwin":
        # On MacOS, CPU PyTorch is hanging with parallel
        os.environ['OMP_NUM_THREADS'] = "1"
    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        sw.data.download_subset(args.use_case, dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))[:7]
        image_files = sorted(glob.glob(output_directory +
                            dataset_name + "/images/*.nrrd"))[:7]
        plane_files = sorted(glob.glob(output_directory +
                            dataset_name + "/constraints/*.json"))[:7]
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))
        image_files = sorted(glob.glob(output_directory + 
                            dataset_name + "/images/*.nrrd"))
        plane_files = sorted(glob.glob(output_directory +
                            dataset_name + "/constraints/*.json"))

    ######################################################################################
    print("\nStep 2. Defining Split")
    """
    Step 2: Define random split for train (70%), validation (15%), and test (15%) sets
    """
    # Shuffle files
    random.shuffle(mesh_files)
    # Get size of validation and test sets
    test_val_size = int(math.ceil(len(mesh_files)*.15))
    # Split data
    test_mesh_files = sorted(mesh_files[:test_val_size])
    val_mesh_files = sorted(mesh_files[test_val_size: test_val_size*2])
    train_mesh_files = sorted(mesh_files[test_val_size*2:])
    print(str(len(train_mesh_files))+" in train set, "+str(len(val_mesh_files))+
            " in validation set, and "+str(len(test_mesh_files))+" in test set")

    ######################################################################################
    print("\nStep 3. Find Training Mesh Transforms")
    """
    Step 3: Train Mesh Grooming 
    The required grooming steps are:
    1. Load mesh
    2. Apply clipping with planes for finding alignment transform
    3. Find reflection tansfrom
    4. Select reference mesh
    5. Find rigid alignment transform
    For more information on grooming see docs/workflow/groom.md
    http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
    """

    # Create a directory for groomed output
    data_dir = output_directory + 'data/'
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)

    """
    To begin grooming, we loop over the files and load the meshes
    """
    ref_side = "L" # chosen so reflection happens in tiny test 
    train_reflections = [] 
    train_names = []
    train_mesh_list = []
    train_plane_files = []
    print('Loading meshes...')
    for train_mesh_filename in train_mesh_files:
        # Get shape name
        train_name = os.path.basename(train_mesh_filename).replace('.ply', '')
        train_names.append(train_name)
        # Get mesh
        train_mesh = sw.Mesh(train_mesh_filename)
        train_mesh_list.append(train_mesh)
        """
        Grooming step 2: Apply clipping for finsing alignment transform
        """
        # Load plane
        for plane_file in plane_files:
            if train_name in plane_file:
                corresponding_plane_file = plane_file
        with open(corresponding_plane_file) as json_file:
            plane = json.load(json_file)['planes'][0]['points']
        train_plane_files.append(corresponding_plane_file)
        # Clip mesh
        train_mesh.clip(plane[0], plane[1], plane[2])
        """
        Grooming Step 3: Get reflection transform - We have left and 
        right femurs, so we reflect the non-reference side meshes 
        so that all of the femurs can be aligned.
        """
        reflection = np.eye(4) # Identity
        if ref_side in train_name:
            reflection[0][0] = -1 # Reflect across X
            train_mesh.applyTransform(reflection)
        train_reflections.append(reflection)

    """
    Grooming Step 4: Select a reference
    This step requires loading all of the meshes at once so the shape
    closest to the mean can be found and selected as the reference. 
    """
    ref_index = sw.find_reference_mesh_index(train_mesh_list)
    # Make a copy of the reference mesh
    ref_mesh = train_mesh_list[ref_index].copy()
    ref_name = train_names[ref_index]
    ref_translate = ref_mesh.center()
    ref_mesh.translate(-ref_translate)
    ref_mesh.write(data_dir + 'reference.vtk')

    print('Creating alignment transforms to ' + ref_name)
    train_rigid_transforms = [] # save in case grooming images
    for train_mesh, train_name in zip(train_mesh_list, train_names):
        """
        Grooming Step 5: Rigid alignment
        This step rigidly aligns each shape to the selected reference. 
        """
        # compute rigid transformation
        rigid_transform = train_mesh.createTransform(ref_mesh, 
                                        sw.Mesh.AlignmentType.Rigid, 100)
        # apply rigid transform
        train_rigid_transforms.append(rigid_transform)
        train_mesh.applyTransform(rigid_transform)

    # Combine transforms to pass to optimizer
    train_transforms = []
    for reflection, rigid_transform in zip(train_reflections, train_rigid_transforms):
        train_transform = np.matmul(rigid_transform, reflection)
        train_transforms.append(train_transform)
    print("Training mesh transforms found.")

    ######################################################################################
    print("\nStep 4. Optimize Training Particles")
    # Create project spreadsheet
    project_location = data_dir
    # Set subjects
    subjects = []
    number_domains = 1
    for i in range(len(train_mesh_list)):
        subject = sw.Subject()
        subject.set_number_of_domains(1)
        rel_mesh_files = sw.utils.get_relative_paths([train_mesh_files[i]], project_location)
        subject.set_original_filenames(rel_mesh_files)
        rel_groom_files = sw.utils.get_relative_paths([train_mesh_files[i]], project_location)
        subject.set_groomed_filenames(rel_groom_files)
        transform = [ train_transforms[i].flatten() ]
        subject.set_groomed_transforms(transform)
        rel_plane_files = sw.utils.get_relative_paths([train_plane_files[i]], project_location)
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
        "initial_relative_weighting" : 0.1,
        "procrustes" : 1,
        "procrustes_interval" : 1,
        "procrustes_scaling" : 1,
        "save_init_splits" : 1,
        "debug_projection" : 0,
        "verbosity" : 0,
        "use_statistics_in_init" : 0,
        "adaptivity_mode": 0
    } 
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 25
        parameter_dictionary["iterations_per_split"] = 25

    # for key in parameter_dictionary:
    #     parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    # parameters.set("domain_type",sw.Variant('mesh'))
    # project.set_parameters("optimize",parameters)
    # spreadsheet_file = data_dir + "train.xlsx"
    # project.save(spreadsheet_file)

    # # Run optimization
    # optimizeCmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    # subprocess.check_call(optimizeCmd)

    # # Analyze - open in studio
    # AnalysisCmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    # subprocess.check_call(AnalysisCmd)

    # Get transforms and particle files from updated project spreadsheet
    spreadsheet_file = '/home/sci/jadie/ShapeWorks/Examples/Python/Output/deep_ssm/data/train.xlsx'
    project = sw.Project()
    project.load(spreadsheet_file)
    train_alignments = [[float(x) for x in s.split()] for s in project.get_string_column("alignment_1")]
    train_procrustes = [[float(x) for x in s.split()] for s in project.get_string_column("procrustes_1")]
    train_local_particles = project.get_string_column("local_particles_1")
    train_world_particles = [x.replace("./", data_dir) for x in project.get_string_column("world_particles_1")]


    ##########################################################################################################
    """
    Step 5: Load training images and apply transforms
    """
    print("\nStep 5. Groom Training Images")
    # Get transfroms from spreadsheet
    train_image_list = []
    print("Preparing training images...")
    for train_name in train_names:
        ID = train_name.split("_")[0]
        # Get corresponding image
        for index in range(len(image_files)):
            if ID in image_files[index]:
                corresponding_image_file = image_files[index]
                break
        train_image = sw.Image(corresponding_image_file)
        train_image_list.append(train_image)
    # Get reference image
    ref_image = train_image_list[ref_index].copy()
    ref_image.resample([1,1,1], sw.InterpolationType.Linear)
    ref_image.setOrigin(ref_image.origin() - ref_translate)
    ref_image.write(data_dir + 'reference_image.nrrd')
    ref_procrustes = sw.utils.getITKtransform(np.array(train_procrustes[ref_index]).reshape(4, 4))
    # Applying rigid transform
    for train_image, train_align, train_proc in zip(train_image_list, train_alignments, train_procrustes):
        train_transform = np.matmul(np.array(train_proc).reshape(4, 4), np.array(train_align).reshape(4, 4))
        train_image.applyTransform(train_transform,
                             ref_image.origin(),  ref_image.dims(),
                             ref_image.spacing(), ref_image.coordsys(),
                             sw.InterpolationType.Linear, meshTransform=True)
    # Get bounding box
    bounding_box = sw.MeshUtils.boundingBox(train_mesh_list)
    bounding_box.pad(10)
    # Crop images
    for train_image in train_image_list:
        train_image.crop(bounding_box)
    # Write images
    print("Writing groomed train images.")
    train_image_files = sw.utils.save_images(data_dir + 'train_images/', train_image_list,
                    train_names, extension='nrrd', compressed=True, verbose=False)

    ##########################################################################################################
    """
    Step 6: Create additional training data
    """
    print("\nStep 6. Augment data\n")
    '''
    - num_samples is how many samples to generate 
    - num_dim is the number of PCA scores to use
    - percent_dim what percent of variablity to retain (used if num_dim is 0)
    - sampler_type is the distribution to use for sampling. Can be gaussian, mixture, or kde
    '''
    num_samples = 4 # 4960
    num_dim = 0
    percent_variability = 0.95
    sampler_type = "kde"
    if args.tiny_test:
        num_samples = 2
        num_dim = 0
        percent_variability = 0.99
    aug_dir = data_dir + "augmentation/"
    embedded_dim = DataAugmentationUtils.runDataAugmentation(aug_dir, train_image_files, train_world_particles,
                                                             num_samples, num_dim, percent_variability, sampler_type,
                                                             mixture_num=0, processes=1)
    print("Dimensions retained: " + str(embedded_dim))
    aug_data_csv = aug_dir + "TotalData.csv"

    if not args.tiny_test and not args.verify:
        DataAugmentationUtils.visualizeAugmentation(aug_data_csv, "violin")

    ######################################################################################
    print("\nStep 7. Find Validation Mesh Transforms")
    """
    Step 7: Validation Mesh Grooming 
    The required grooming steps are:
    1. Load mesh
    2. Apply clipping with planes for finding alignment transform
    3. Find reflection tansfrom
    4. Find rigid alignment transform
    """

    """
    To begin grooming, we loop over the files and load the meshes
    """
    val_reflections = [] 
    val_names = []
    val_mesh_list = []
    val_plane_files = []
    print('Loading meshes...')
    for val_mesh_filename in val_mesh_files:
        # Get shape name
        val_name = os.path.basename(val_mesh_filename).replace('.ply', '')
        val_names.append(val_name)
        # Get mesh
        val_mesh = sw.Mesh(val_mesh_filename)
        val_mesh_list.append(val_mesh)
        """
        Grooming step 2: Apply clipping for finsing alignment transform
        """
        # Load plane
        for plane_file in plane_files:
            if val_name in plane_file:
                corresponding_plane_file = plane_file
        with open(corresponding_plane_file) as json_file:
            plane = json.load(json_file)['planes'][0]['points']
        val_plane_files.append(corresponding_plane_file)
        # Clip mesh
        val_mesh.clip(plane[0], plane[1], plane[2])
        """
        Grooming Step 3: Get reflection transform - We have left and 
        right femurs, so we reflect the non-reference side meshes 
        so that all of the femurs can be aligned.
        """
        reflection = np.eye(4) # Identity
        if ref_side in val_name:
            reflection[0][0] = -1 # Reflect across X
            val_mesh.applyTransform(reflection)
        val_reflections.append(reflection)

    print('Creating alignment transforms to ' + ref_name)
    val_rigid_transforms = [] # save in case grooming images
    for val_mesh, val_name in zip(val_mesh_list, val_names):
        """
        Grooming Step 4: Rigid alignment
        This step rigidly aligns each shape to the selected reference. 
        """
        # compute rigid transformation
        rigid_transform = val_mesh.createTransform(ref_mesh, 
                                        sw.Mesh.AlignmentType.Rigid, 100)
        # apply rigid transform
        val_rigid_transforms.append(rigid_transform)
        val_mesh.applyTransform(rigid_transform)

    # Combine transforms to pass to optimizer
    val_transforms = []
    for reflection, rigid_transform in zip(val_reflections, val_rigid_transforms):
        val_transform = np.matmul(rigid_transform, reflection)
        val_transforms.append(val_transform)
    print("Validation mesh transforms found.")


    #################################################################################################
    print("\nStep 8. Optimize Validation Particles with Fixed Domains")
    # Get mean shape 
    train_model_dir = data_dir + "/train_particles/"
    file_list = sorted(glob.glob(train_model_dir + '/*world.particles'))
    mean_shape = np.loadtxt(file_list[0])
    for i in range(1,len(file_list)):
        mean_shape += np.loadtxt(file_list[i])
    mean_shape = mean_shape / len(file_list)
    mean_shape_path = train_model_dir + '/meanshape_local.particles'
    np.savetxt(mean_shape_path, mean_shape)
    # Create spreadsheet
    project_location = data_dir
    subjects = []
    # Add fixed training shapes
    for i in range(len(train_mesh_list)):
        subject = sw.Subject()
        subject.set_number_of_domains(1)
        rel_mesh_files = sw.utils.get_relative_paths([train_mesh_files[i]], project_location)
        rel_groom_files = sw.utils.get_relative_paths([train_mesh_files[i]], project_location)
        rel_plane_files = sw.utils.get_relative_paths([train_plane_files[i]], project_location)
        subject.set_original_filenames(rel_mesh_files)
        subject.set_groomed_filenames(rel_groom_files)
        transform = [ train_transforms[i].flatten() ]
        subject.set_groomed_transforms(transform)
        subject.set_constraints_filenames(rel_plane_files)
        subject.set_landmarks_filenames([train_local_particles[i]])
        subject.set_extra_values({"fixed": "yes"})
        subjects.append(subject)
    # Add new validation shapes
    for i in range(len(val_mesh_list)):
        subject = sw.Subject()
        subject.set_number_of_domains(1)
        rel_mesh_files = sw.utils.get_relative_paths([val_mesh_files[i]], project_location)
        rel_groom_files = sw.utils.get_relative_paths([val_mesh_files[i]], project_location)
        rel_plane_files = sw.utils.get_relative_paths([val_plane_files[i]], project_location)
        # rel_particle_files = sw.utils.get_relative_paths([mean_shape_path], project_location)
        subject.set_original_filenames(rel_mesh_files)
        subject.set_groomed_filenames(rel_groom_files)
        transform = [ val_transforms[i].flatten() ]
        subject.set_groomed_transforms(transform)
        subject.set_constraints_filenames(rel_plane_files)
        # subject.set_landmarks_filenames(rel_particle_files)
        subject.set_extra_values({"fixed": "no"})
        subjects.append(subject)
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Update parameter dictionary
    parameter_dictionary["use_landmarks"] = 1
    parameter_dictionary["use_fixed_subjects"] = 1
    parameter_dictionary["narrow_band"] = 1e10
    parameter_dictionary["fixed_subjects_column"] = "fixed"
    parameter_dictionary["fixed_subjects_choice"] = "yes"
    parameter_dictionary["verbosity"] = 3
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant(parameter_dictionary[key]))
    project.set_parameters("optimize", parameters)
    # Set studio parameters
    studio_dictionary = {
        "show_landmarks": 0,
        "tool_state": "analysis"
    }
    studio_parameters = sw.Parameters()
    for key in studio_dictionary:
        studio_parameters.set(key, sw.Variant(studio_dictionary[key]))
    project.set_parameters("studio", studio_parameters)
    spreadsheet_file = data_dir + "validation.xlsx"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    subprocess.check_call(optimize_cmd)

    analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(analyze_cmd)

    # ##########################################################################################################
    # print("\nStep 5. Find Test and Validation Transforms and Groom Images")
    # """
    # Step 5: Find test and validation transforms and images
    # 1. Find reflection
    # 2. Find alignment transform using rigid image registration
    # 3. Apply alignment to image
    # 4. Crop image
    # """
    # # Helper function - Works best if moving and fixed images have been centered
    # def rigid_image_registration_transform(fixed_image_file, moving_image_file, out_image_file):
    #     fixed_image = itk.imread(fixed_image_file, itk.F)
    #     moving_image = itk.imread(moving_image_file, itk.F)
    #     # Import Default Parameter Map
    #     parameter_object = itk.ParameterObject.New()
    #     default_rigid_parameter_map = parameter_object.GetDefaultParameterMap('rigid')
    #     parameter_object.AddParameterMap(default_rigid_parameter_map)
    #     # Call registration function
    #     result_image, result_transform_parameters = itk.elastix_registration_method(
    #         fixed_image, moving_image, parameter_object=parameter_object)
    #     itk.imwrite(result_image, out_image_file)
    #     # Get transform matrix - can't find a way to do it without writing it
    #     result_transform_parameters.WriteParameterFile("temp.txt")
    #     with open("temp.txt") as f:
    #         lines = f.readlines()
    #     transform_params = lines[-2].replace(")\n","").split(" ")[1:]
    #     transform_params = np.array(transform_params).astype(np.float)
    #     thetas = transform_params[:3]
    #     rotation_matrix = scipy.spatial.transform.Rotation.from_euler('XYZ',thetas).as_matrix()
    #     translation = transform_params[3:]
    #     itk_transform_matrix = np.eye(4)
    #     itk_transform_matrix[:3,:3] = rotation_matrix
    #     itk_transform_matrix[-1,:3] = translation
    #     # debug
    #     ref_image = sw.Image(fixed_image_file)
    #     sw.Image(moving_image_file).applyTransform(itk_transform_matrix,
    #                          ref_image.origin(),  ref_image.dims(),
    #                          ref_image.spacing(), ref_image.coordsys(),
    #                          sw.InterpolationType.Linear, meshTransform=False).write(out_image_file.replace(".nrrd","2.nrrd"))
    #     vtk_transform_matrix = sw.utils.getVTKtransform(itk_transform_matrix)
    #     # os.remove("temp.txt")
    #     return vtk_transform_matrix
    # # Get reference
    # ref_image_file = data_dir + 'reference_image.nrrd'
    # ref_image = sw.Image(ref_image_file)
    # padded_bb = sw.PhysicalRegion(bounding_box.min, bounding_box.max).pad(30)
    # cropped_ref_image_file = data_dir + 'roughly_cropped_reference_image.nrrd'
    # cropped_ref_image = sw.Image(ref_image_file).crop(padded_bb).write(cropped_ref_image_file)
    # ref_center = ref_image.center() # get center
    # ref_side = "L" # chosen arbitrarily
    # # Make dirs 
    # val_test_images_dir = data_dir + 'val_and_test_images/'
    # if not os.path.exists(val_test_images_dir):
    #     os.makedirs(val_test_images_dir)
    # val_test_transforms_dir = data_dir + 'val_and_test_transforms/'
    # if not os.path.exists(val_test_transforms_dir):
    #     os.makedirs(val_test_transforms_dir)
    # # Combine mesh files
    # val_test_mesh_files = val_mesh_files + test_mesh_files
    # # Loop over
    # print("Grooming validation and test images...")
    # val_test_images = []
    # val_test_transforms = []
    # for vt_mesh_file in val_test_mesh_files:
    #     # Get name
    #     vt_name = os.path.basename(vt_mesh_file).replace('.ply', '')
    #     ID = vt_name.split("_")[0]
    #     # Get corresponding image
    #     for index in range(len(image_files)):
    #         if ID in image_files[index]:
    #             corresponding_image_file = image_files[index]
    #             break
    #     vt_image = sw.Image(corresponding_image_file)
    #     val_test_images.append(vt_image)
    #     # Apply reflection transform 
    #     reflection = np.eye(4) # Identity
    #     if ref_side in vt_name:
    #         reflection[0][0] = -1 # Reflect across X
    #     vt_image.applyTransform(reflection)
    #     # Translate to make rigid registration easier
    #     translation = ref_center - vt_image.center()
    #     vt_image.setOrigin(vt_image.origin() + translation)
    #     vt_image_file = val_test_images_dir + vt_name + ".nrrd"
    #     vt_image.write(vt_image_file)
    #     # Get rigid transform from image registration and apply
    #     # Roughly align, roughly crop, then realign
    #     print("Finding rigid transform for: " + vt_name)
    #     rigid_transform1 = rigid_image_registration_transform(ref_image_file, vt_image_file, vt_image_file)
    #     # vt_image = sw.Image(vt_image_file).crop(padded_bb).write(vt_image_file)
    #     # rigid_transform2 = rigid_image_registration_transform(cropped_ref_image_file, vt_image_file, vt_image_file)
    #     # Crop
    #     sw.Image(vt_image_file).crop(bounding_box).write(vt_image_file)
    #     # Get combined transform and write
    #     translation_marix = np.eye(4)
    #     translation_marix[:3,-1] = translation
    #     transform = np.matmul(translation_marix, reflection)
    #     transform = np.matmul(rigid_transform1, transform)
    #     # transform = np.matmul(rigid_transform2, transform)
    #     val_test_transforms.append(transform)
    #     np.save(val_test_transforms_dir + ID + '.npy', transform)        



    
    # # debug
    # groomed_meshes = []
    # names = []
    # for mesh_file, transform in zip(train_mesh_files, train_transforms):
    #   names.append(mesh_file.split('/')[-1].replace(".ply",""))
    #   groomed_meshes.append(sw.Mesh(mesh_file).applyTransform(transform))
    # m_files = sw.utils.save_meshes(data_dir + 'debug_train_meshes/', groomed_meshes, names)
    # groomed_meshes = []
    # names = []
    # for mesh_file, transform in zip(val_test_mesh_files, val_test_transforms):
    #   names.append(mesh_file.split('/')[-1].replace(".ply",""))
    #   groomed_meshes.append(sw.Mesh(mesh_file).applyTransform(transform))
    # m_files = sw.utils.save_meshes(data_dir + 'debug_val_test_meshes/', groomed_meshes, names)


    # print("\nStep 7. Optimize Validation and Test Particles Using Fixed Domain")
    # """
    # TODO: Make this fixed domain!!
    # Step 7: FIXED DOMAIN OPTIMIZE - Particle based optimization for validation and test using fixed domains
    # Particles for training meshes will not be updated
    # Local particles will be in alignment with original images and meshes
    # World particles would be in alignment with groomed images from the previous step if Procrustes was set to off
    # Visit this link for more information about optimization: 
    # http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    # """
    # # Create spreadsheet
    # subjects = []
    # for i in range(len(train_mesh_files)):
    #     subject = sw.Subject()
    #     subject.set_number_of_domains(1)
    #     subject.set_segmentation_filenames([train_mesh_files[i]])
    #     subject.set_groomed_filenames([train_mesh_files[i]])
    #     subject.set_groomed_transforms([train_transforms[i].flatten()])
    #     subjects.append(subject)
    # for i in range(len(val_test_mesh_files)):
    #     subject = sw.Subject()
    #     subject.set_number_of_domains(1)
    #     subject.set_segmentation_filenames([val_test_mesh_files[i]])
    #     subject.set_groomed_filenames([val_test_mesh_files[i]])
    #     subject.set_groomed_transforms([val_test_transforms[i].flatten()])
    #     subjects.append(subject)
    # project = sw.Project()
    # project.set_subjects(subjects)
    # parameters = sw.Parameters()
    # # Create a dictionary for all the parameters required by optimization
    # parameter_dictionary = {
    #     "number_of_particles" : 512,
    #     "use_normals": 0,
    #     "normal_weight": 10.0,
    #     "checkpointing_interval" : 200,
    #     "keep_checkpoints" : 0,
    #     "iterations_per_split" : 1000,
    #     "optimization_iterations" : 500,
    #     "starting_regularization" : 100,
    #     "ending_regularization" : 0.1,
    #     "recompute_regularization_interval" : 2,
    #     "domains_per_shape" : 1,
    #     "relative_weighting" : 10,
    #     "initial_relative_weighting" : 0.01,
    #     "procrustes_interval" : 1,
    #     "procrustes_scaling" : 1,
    #     "save_init_splits" : 1,
    #     "debug_projection" : 0,
    #     "verbosity" : 0,
    #     "use_statistics_in_init" : 0,
    #     "adaptivity_mode": 0
    # }  
    # # Set params and save spreadsheet
    # for key in parameter_dictionary:
    #     parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    # parameters.set("domain_type",sw.Variant('mesh'))
    # project.set_parameters("optimze",parameters)
    # spreadsheet_file = data_dir + "val_test_optimize.xlsx"
    # project.save(spreadsheet_file)
    # # Optimize 
    # optimizeCmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    # subprocess.check_call(optimizeCmd)
    # # Analyze
    # AnalysisCmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    # subprocess.check_call(AnalysisCmd)