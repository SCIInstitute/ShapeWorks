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
import SimpleITK
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
        parameter_dictionary["number_of_particles"] = 128 # 32
        parameter_dictionary["optimization_iterations"] = 25

    for key in parameter_dictionary:
        parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    parameters.set("domain_type",sw.Variant('mesh'))
    project.set_parameters("optimize",parameters)
    spreadsheet_file = data_dir + "train.xlsx"
    project.save(spreadsheet_file)

    # Run optimization
    optimizeCmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    subprocess.check_call(optimizeCmd)

    print("To analyze train shape model, call:")
    print(" ShapeWorksStudio " + spreadsheet_file)

    # # TODO remove - Analyze - open in studio
    # AnalysisCmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    # subprocess.check_call(AnalysisCmd)

    # Get transforms and particle files from updated project spreadsheet
    # spreadsheet_file = '/home/sci/jadie/ShapeWorks/Examples/Python/Output/deep_ssm/data/train.xlsx'
    project = sw.Project()
    project.load(spreadsheet_file)
    train_alignments = [[float(x) for x in s.split()] for s in project.get_string_column("alignment_1")]
    train_alignments = [np.array(x).reshape(4, 4) for x in train_alignments] # reshape
    train_procrustes = [[float(x) for x in s.split()] for s in project.get_string_column("procrustes_1")]
    train_procrustes = [np.array(x).reshape(4, 4) for x in train_procrustes] #reshape
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
    if ref_side in train_names[ref_index]:
        print("Reflecting ref image.")
        reflection = np.eye(4) # Identity
        reflection[0][0] = -1 # Reflect across X
        ref_image.applyTransform(reflection)
    ref_image.resample([1,1,1], sw.InterpolationType.Linear)
    ref_image.setOrigin(ref_image.origin() - ref_translate)
    ref_image.write(data_dir + 'reference_image.nrrd')
    ref_procrustes = sw.utils.getITKtransform(train_procrustes[ref_index])
    # Applying rigid transform
    for train_image, train_align, train_proc in zip(train_image_list, train_alignments, train_procrustes):
        train_transform = np.matmul(train_proc, train_align)
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

    ###########################################################################################
    """
    Step 6: Create additional training data
    """
    print("\nStep 6. Augment data")
    '''
    - num_samples is how many samples to generate 
    - num_dim is the number of PCA scores to use
    - percent_dim what percent of variablity to retain (used if num_dim is 0)
    - sampler is the distribution to use for sampling. Can be gaussian, mixture, or kde
    '''
    num_samples = 4 # 4960
    num_dim = 0
    percent_variability = 0.95
    sampler = "kde"
    if args.tiny_test:
        num_samples = 2
        num_dim = 0
        percent_variability = 0.99
    aug_dir = data_dir + "augmentation/"
    embedded_dim = DataAugmentationUtils.runDataAugmentation(aug_dir, train_image_files, 
                                                            train_world_particles, num_samples, 
                                                            num_dim, percent_variability, 
                                                            sampler, mixture_num=0, processes=1)
    print("Dimensions retained: " + str(embedded_dim))
    aug_data_csv = aug_dir + "TotalData.csv"

    if not args.tiny_test and not args.verify:
        DataAugmentationUtils.visualizeAugmentation(aug_data_csv, "violin")

    #############################################################################################
    print("\nStep 7. Find Test and Validation Transforms and Groom Images")
    """
    Step 7: Find test and validation transforms and images
    1. Find reflection
    2. Find alignment transform using rigid image registration
    3. Apply alignment to image
    4. Crop image
    """
    """
    TODO - move to pymodule
    Helper function - Works best if moving and fixed images have been centered
    type can either be rigid or similarity (rigid + isotropic scale), default is rigid
    Returns vtk version of transform matrix
    """
    def image_registration_transform(fixed_image_file, moving_image_file, out_image_file, transform_type='rigid'):
        ref_image = sw.Image(fixed_image_file)
        image = sw.Image(moving_image_file)
        # Import Default Parameter Map 
        parameter_object = itk.ParameterObject.New()
        parameter_map = parameter_object.GetDefaultParameterMap('rigid')
        if transform_type == 'similarity':
            parameter_map['Transform'] = ['SimilarityTransform']
        parameter_object.AddParameterMap(parameter_map)
        # Call registration function
        fixed_image = itk.imread(fixed_image_file, itk.F)
        moving_image = itk.imread(moving_image_file, itk.F)
        result_image, result_transform_parameters = itk.elastix_registration_method(
            fixed_image, moving_image, parameter_object=parameter_object)
        # Get transform matrix
        parameter_map = result_transform_parameters.GetParameterMap(0)
        transform_params = np.array(parameter_map['TransformParameters'], dtype=float)
        if transform_type == 'rigid':
            itk_transform = SimpleITK.Euler3DTransform()
        elif transform_type == 'similarity':
            itk_transform = SimpleITK.Similarity3DTransform()
        else:
            print("Error: " + transform_type + " transform unimplemented.")
        itk_transform.SetParameters(transform_params)
        itk_transform_matrix = np.eye(4)
        itk_transform_matrix[:3,:3] = np.array(itk_transform.GetMatrix()).reshape(3,3)
        itk_transform_matrix[-1,:3] = np.array(itk_transform.GetTranslation())
        # Apply transform
        image.applyTransform(itk_transform_matrix,
                             ref_image.origin(),  ref_image.dims(),
                             ref_image.spacing(), ref_image.coordsys(),
                             sw.InterpolationType.Linear, 
                             meshTransform=False).write(out_image_file)
        return sw.utils.getVTKtransform(itk_transform_matrix)
    
    # Get reference
    ref_image_file = data_dir + 'reference_image.nrrd'
    ref_image = sw.Image(ref_image_file)
    padded_bb = sw.PhysicalRegion(bounding_box.min, bounding_box.max).pad(30)
    cropped_ref_image_file = data_dir + 'roughly_cropped_reference_image.nrrd'
    cropped_ref_image = sw.Image(ref_image_file).crop(padded_bb).write(cropped_ref_image_file)
    ref_center = ref_image.center() # get center
    ref_side = "L" # chosen arbitrarily
    # Make dirs 
    val_test_images_dir = data_dir + 'val_and_test_images/'
    if not os.path.exists(val_test_images_dir):
        os.makedirs(val_test_images_dir)
    # Combine mesh files
    val_test_mesh_files = val_mesh_files + test_mesh_files
    # Loop over
    print("Grooming validation and test images...")
    val_test_image_files = []
    val_test_transforms = []
    for vt_mesh_file in val_test_mesh_files:
        # Get name
        vt_name = os.path.basename(vt_mesh_file).replace('.ply', '')
        ID = vt_name.split("_")[0]
        # Get corresponding image
        for index in range(len(image_files)):
            if ID in image_files[index]:
                corresponding_image_file = image_files[index]
                break
        vt_image = sw.Image(corresponding_image_file)
        vt_image_file = val_test_images_dir + vt_name + ".nrrd"
        val_test_image_files.append(vt_image_file)
        # Apply reflection transform 
        reflection = np.eye(4) # Identity
        if ref_side in vt_name:
            reflection[0][0] = -1 # Reflect across X
        vt_image.applyTransform(reflection)
        # Translate to make rigid registration easier
        translation = ref_center - vt_image.center()
        vt_image.setOrigin(vt_image.origin() + translation).write(vt_image_file)
        # Get initial rigid transform from image registration and apply
        rigid_transform = image_registration_transform(ref_image_file, vt_image_file, 
                                vt_image_file, transform_type='rigid')
        # Apply rough cropping to image
        sw.Image(vt_image_file).crop(padded_bb).write(vt_image_file)
        # Get similarity transform from image registration and apply
        similarity_transform = image_registration_transform(cropped_ref_image_file, 
                                vt_image_file, vt_image_file, transform_type='similarity')
        # Apply final cropping to image
        sw.Image(vt_image_file).crop(bounding_box).write(vt_image_file)
        # Combine transforms to get single matrix and save
        translation_marix = np.eye(4)
        translation_marix[:3,-1] = translation
        transform = np.matmul(similarity_transform, np.matmul(rigid_transform, 
                    np.matmul(translation_marix, reflection)))
        val_test_transforms.append(transform)
    # split val and test groomed images and transforms    
    val_image_files = val_test_image_files[:len(val_mesh_files)]
    val_transforms = val_test_transforms[:len(val_mesh_files)]
    test_image_files = val_test_image_files[len(val_mesh_files):]
    test_transforms = val_test_transforms[len(val_mesh_files):]


    #################################################################################################
    print("\nStep 8. Optimize Validation Particles with Fixed Domains")
    # Get validation plane files
    val_plane_files = []
    val_names = []
    for val_mesh_filename in val_mesh_files:
        val_name = os.path.basename(val_mesh_filename).replace('.ply', '')
        val_names.append(val_name)
        for plane_file in plane_files:
            if val_name in plane_file:
                corresponding_plane_file = plane_file
        val_plane_files.append(corresponding_plane_file)

    # Get meanshape in world particles
    mean_shape = np.loadtxt(train_world_particles[0])
    for i in range(1,len(train_world_particles)):
        mean_shape += np.loadtxt(train_world_particles[i])
    mean_shape = mean_shape / len(train_world_particles)
    np.savetxt(data_dir + '/meanshape_world.particles', mean_shape)
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
    if not os.path.exists(data_dir + "validation_particles/"):
        os.makedirs(data_dir + "validation_particles/")
    for i in range(len(val_mesh_files)):
        subject = sw.Subject()
        subject.set_number_of_domains(1)
        rel_mesh_files = sw.utils.get_relative_paths([val_mesh_files[i]], project_location)
        rel_groom_files = sw.utils.get_relative_paths([val_mesh_files[i]], project_location)
        rel_plane_files = sw.utils.get_relative_paths([val_plane_files[i]], project_location)
        initial_particles = sw.utils.transformParticles(mean_shape, val_transforms[i], inverse=True)
        initial_particle_file = data_dir + "validation_particles/" + val_names[i] + "_local.particles"
        np.savetxt(initial_particle_file, initial_particles)
        rel_particle_files = sw.utils.get_relative_paths([initial_particle_file], project_location)
        subject.set_original_filenames(rel_mesh_files)
        subject.set_groomed_filenames(rel_groom_files)
        transform = [ val_transforms[i].flatten() ]
        subject.set_groomed_transforms(transform)
        subject.set_constraints_filenames(rel_plane_files)
        subject.set_landmarks_filenames(rel_particle_files)
        subject.set_extra_values({"fixed": "no"})
        subjects.append(subject)
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Update parameter dictionary
    parameter_dictionary["procrustes"] = 0 
    parameter_dictionary["procrustes_interval"] = 0
    parameter_dictionary["procrustes_scaling"] = 0
    parameter_dictionary["use_landmarks"] = 1
    parameter_dictionary["use_fixed_subjects"] = 1
    parameter_dictionary["narrow_band"] = 1e10
    parameter_dictionary["fixed_subjects_column"] = "fixed"
    parameter_dictionary["fixed_subjects_choice"] = "yes"
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

    print("To analyze validation shape model, call:")
    print(" ShapeWorksStudio " + spreadsheet_file)
    
    # # TODO remove - Analyze - open in studio
    # analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    # subprocess.check_call(analyze_cmd)

    # Get validation world particle files
    val_world_particles = []
    for val_name in val_names:
        val_world_particles.append(data_dir + 'validation_particles/' + val_name + "_world.particles")

    # # TODO remove debug
    # for val_mesh_file, val_name, val_transform in zip(val_mesh_files, val_names, val_transforms):
    #     sw.Mesh(val_mesh_file).applyTransform(val_transform).write(data_dir + 'debug/' + val_name + ".vtk")

    #################################################################################################
    print("\nStep 9. Create PyTorch loaders from data.")
    '''
    Create training, validation, and test torch loaders
    If down_sample is true, model will train on images that are smaller by down_factor
    Hyper-parameter batch_size is for training
        Higher batch size will help speed up training but uses more cuda memory, 
        if you get a memory error try reducing the batch size
    '''
    down_factor = 0.75
    batch_size = 8
    loader_dir = output_directory + 'torch_loaders/'
    DeepSSMUtils.getTrainLoader(loader_dir, aug_data_csv, batch_size, down_factor, 
                                down_dir= data_dir + "train_downsampled_images/")
    DeepSSMUtils.getValidationLoader(loader_dir, val_image_files, val_world_particles,  
                                down_factor, down_dir= data_dir + "val_downsampled_images/")
    DeepSSMUtils.getTestLoader(loader_dir, test_image_files, down_factor, 
                                down_dir= data_dir + "test_downsampled_images/")

    print("\nStep 10. Train DeepSSM model.")
    # Define model parameters
    model_name = "femur_deepssm"
    model_parameters = {
        "model_name": model_name,
        "num_latent_dim": int(embedded_dim),
        "paths": {
            "out_dir": output_directory,
            "loader_dir": loader_dir,
            "aug_dir": aug_dir
        },
        "encoder": {
            "deterministic": True
        },
        "decoder": {
            "deterministic": True,
            "linear": True
        },
        "loss": {
            "function": "MSE",
            "supervised_latent": True,
        },
        "trainer": {
            "epochs": 100,
            "learning_rate": 0.001,
            "decay_lr": True,
            "val_freq": 1
        },
        "fine_tune": {
            "enabled": True,
            "loss": "MSE",
            "epochs": 2,
            "learning_rate": 0.001,
            "decay_lr": True,
            "val_freq": 1
        },
        "use_best_model": True
    }
    if args.tiny_test:
        model_parameters["trainer"]["epochs"] = 1
        model_parameters["fine_tune"]["epochs"] = 1
    # Save config file    
    config_file = output_directory + model_name + ".json"
    with open(config_file, "w") as outfile:
        json.dump(model_parameters, outfile, indent=2)
    # Train
    DeepSSMUtils.trainDeepSSM(config_file)


    print("\nStep 11. Predict validation particles with trained DeepSSM and analyze accuracy.")
    '''
    Use trained DeepSSM model to predict validation particles
    and apply inverse transforms to get local predcitions
    '''
    predicted_val_world_particles = DeepSSMUtils.testDeepSSM(config_file, loader='validation')
    print("Validation world predictions saved.")
    # Generate local predictions
    local_val_prediction_dir = output_directory + model_name + '/validation_predictions/local_predictions/'
    if not os.path.exists(local_val_prediction_dir):
        os.makedirs(local_val_prediction_dir)
    predicted_val_local_particles = []
    for particle_file, transform in zip(predicted_val_world_particles, val_transforms):
        particles = np.loadtxt(particle_file)
        local_particle_file = particle_file.replace("FT_Predictions/", "local_predictions/")
        local_particles = sw.utils.transformParticles(particles, transform, inverse=True)
        np.savetxt(local_particle_file, local_particles)
        predicted_val_local_particles.append(local_particle_file)
    print("Validation local predictions written to: " + local_val_prediction_dir)
    '''
    Analyze validation accuracy in terms of:
    - MSE between true and predcited world partcles
    - Surface to surface distance between true mesh and mesh generated from predicted local particles
    '''
    mean_MSE, std_MSE = DeepSSMUtils.analyzeMSE(predicted_val_world_particles, val_world_particles)
    print("Validation world particle MSE: "+str(mean_MSE)+" +- "+str(std_MSE))
    out_dir = output_directory + model_name + "/validation_results/"
    template_mesh = train_mesh_files[ref_index]
    template_particles = train_local_particles[ref_index].replace("./", data_dir)
    mean_dist = DeepSSMUtils.analyzeMeshDistance(predicted_val_local_particles, val_mesh_files, 
                                                    template_particles, template_mesh, out_dir)
    print("Validation mean mesh surface-to-surface distance: "+str(mean_dist))


    print("\nStep 12. Predict test particles with trained DeepSSM and analyze accuracy.")
    '''
    Use trained DeepSSM model to predict test particles
    and apply inverse transforms to get local predcitions
    '''
    predicted_test_world_particles = DeepSSMUtils.testDeepSSM(config_file, loader='test')
    print("Test world predictions saved.")
    # Generate local predictions
    local_test_prediction_dir = output_directory + model_name + '/test_predictions/local_predictions/'
    if not os.path.exists(local_test_prediction_dir):
        os.makedirs(local_test_prediction_dir)
    predicted_test_local_particles = []
    for particle_file, transform in zip(predicted_test_world_particles, test_transforms):
        particles = np.loadtxt(particle_file)
        local_particle_file = particle_file.replace("FT_Predictions/", "local_predictions/")
        local_particles = sw.utils.transformParticles(particles, transform, inverse=True)
        np.savetxt(local_particle_file, local_particles)
        predicted_test_local_particles.append(local_particle_file)
    print("Test local predictions written to: " + local_test_prediction_dir)
    '''
    Analyze test accuracy in terms of surface to surface distance between 
    true mesh and mesh generated from predicted local particles
    '''
    out_dir = output_directory + model_name + "/test_results/"
    mean_dist = DeepSSMUtils.analyzeMeshDistance(predicted_test_local_particles, test_mesh_files, 
                                                    template_particles, template_mesh, out_dir)
    print("Test mean mesh surface-to-surface distance: "+str(mean_dist))

    # # If tiny test or verify, check results and exit
    # if args.tiny_test or args.verify:
    #     if args.tiny_test:
    #         if not os.path.exists("Output/deep_ssm/femur_deepssm/predictions/PCA_Predictions/predicted_pca_m07_L.particles"):
    #             print("tiny test failed")
    #             exit(-1)
    #         # TODO: verify full run
    #     print("Done with test, verification succeeded.")
    #     exit()
