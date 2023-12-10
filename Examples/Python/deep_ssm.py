# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks DeepSSM
====================================================================
"""
import os
import glob
import platform
import torch
import random
import math
import numpy as np
import subprocess
import json
import shapeworks as sw
import DataAugmentationUtils
import DeepSSMUtils
from shapeworks.utils import sw_message

torch.multiprocessing.set_sharing_strategy('file_system')
random.seed(4)


def Run_Pipeline(args):
    sw.setup_console_logging()

    ######################################################################################
    # Step 1. Acquire Data
    ######################################################################################
    print("\nStep 1. Acquire Data")
    """
    Step 1: ACQUIRE DATA
    We define dataset_name which determines which dataset to download from
    the portal and the directory to save output from the use case in.
    This data is comprised of femur meshes and corresponding hip CT scans.
    """
    dataset_name = "deep_ssm_femur"
    output_directory = os.getcwd() + "/Output/deep_ssm/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    if platform.system() == "Darwin":
        # On MacOS, CPU PyTorch is hanging with parallel
        os.environ['OMP_NUM_THREADS'] = "1"
    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "deep_ssm_femur_tiny_test"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "femur"
        mesh_files = sorted(glob.glob(output_directory +
                                      dataset_name + "/meshes/*.ply"))[:5]
        image_files = sorted(glob.glob(output_directory +
                                       dataset_name + "/images/*.nrrd"))[:5]
        plane_files = sorted(glob.glob(output_directory +
                                       dataset_name + "/constraints/*.json"))[:5]
    else:
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "femur"
        mesh_files = sorted(glob.glob(output_directory +
                                      dataset_name + "/meshes/*.ply"))
        image_files = sorted(glob.glob(output_directory +
                                       dataset_name + "/images/*.nrrd"))
        plane_files = sorted(glob.glob(output_directory +
                                       dataset_name + "/constraints/*.json"))

    ######################################################################################
    # Step 2. Construct ShapeWorks Project
    ######################################################################################

    # Create a directory for groomed output
    data_dir = output_directory + 'data/'
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)

    ref_side = "L"  # chosen so reflection happens in tiny test

    # Create Subjects
    subjects = []
    for i in range(len(mesh_files)):
        subject = sw.Subject()
        subject.set_original_filenames([mesh_files[i]])
        subject.set_constraints_filenames([plane_files[i]])
        subject.set_feature_filenames({"CT": image_files[i]})
        if ref_side in mesh_files[i]:
            subject.set_extra_values({"side": "L"})
        else:
            subject.set_extra_values({"side": "R"})
        subjects.append(subject)
    # Create ShapeWorks Project
    project = sw.Project()
    project.set_subjects(subjects)

    # Save project spreadsheet
    spreadsheet_file = data_dir + "deepssm_project.xlsx"
    project.save(spreadsheet_file)

    ######################################################################################
    # Step 3. Define Split
    ######################################################################################

    DeepSSMUtils.create_split(project, 80, 10, 10)
    project.save(spreadsheet_file)

    ######################################################################################
    # Step 4. Groom Training Data
    ######################################################################################

    # Set to ICP
    params = project.get_parameters("groom")
    params.set("alignment_method", "Iterative Closest Point")
    params.set("alignment_subset_size", "20")
    params.set("remesh", "0")
    project.set_parameters("groom", params)

    sw_message("Grooming training data...")
    DeepSSMUtils.groom_training_data(project)
    project.save(spreadsheet_file)

    reference_index = DeepSSMUtils.get_reference_index(project)
    print("Reference index: " + str(reference_index))

    ######################################################################################
    # Step 5. Optimize Training Particles
    ######################################################################################

    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 512,
        "use_normals": 0,
        "normals_strength": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 1500,
        "optimization_iterations": 1000,
        "starting_regularization": 200,
        "ending_regularization": 0.1,
        "relative_weighting": 10,
        "initial_relative_weighting": 0.1,
        "procrustes": 1,
        "procrustes_interval": 1,
        "procrustes_scaling": 1,
        "save_init_splits": 1,
        "verbosity": 0,
    }
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 128
        parameter_dictionary["optimization_iterations"] = 25
    # Run multi-scale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["multiscale"] = 1
        parameter_dictionary["multiscale_particles"] = 64

    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    project.set_parameters("optimize", parameters)
    spreadsheet_file = data_dir + "train.xlsx"
    project.save(spreadsheet_file)

    sw_message("Optimize training particles...")
    optimize = sw.Optimize()
    optimize.SetUpOptimize(project)
    optimize.run()

    # exit
    return

    # Get transforms and particle files from updated project spreadsheet
    project = sw.Project()
    project.load(spreadsheet_file)
    train_alignments = [[float(x) for x in s.split()] for s in project.get_string_column("alignment_1")]
    train_alignments = [np.array(x).reshape(4, 4) for x in train_alignments]  # reshape
    train_procrustes = [[float(x) for x in s.split()] for s in project.get_string_column("procrustes_1")]
    train_procrustes = [np.array(x).reshape(4, 4) for x in train_procrustes]  # reshape
    train_local_particles = project.get_string_column("local_particles_1")
    train_world_particles = [x.replace("./", data_dir) for x in project.get_string_column("world_particles_1")]

    ######################################################################################
    print("\nStep 5. Groom Training Images")
    """
    Step 5: Prep training images                        
    This includes:
    - Creating a reference image
    - Applying transforms found in step 3
    - Crop using bounding box 
    """
    # Get transforms from spreadsheet
    train_image_list = []
    train_transforms = []
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
    # Get reference image and transform it so it matches reference mesh
    ref_image = train_image_list[ref_index].copy()
    if ref_side in train_names[ref_index]:
        reflection = np.eye(4)  # Identity
        reflection[0][0] = -1  # Reflect across X
        ref_image.applyTransform(reflection)
    ref_image.resample([1, 1, 1], sw.InterpolationType.Linear)
    ref_image.setOrigin(ref_image.origin() - ref_translate)
    ref_image.write(data_dir + 'reference_image.nrrd')
    ref_procrustes = sw.utils.getITKtransform(train_procrustes[ref_index])
    # Applying rigid transform
    for train_image, train_align, train_proc in zip(train_image_list, train_alignments, train_procrustes):
        train_transform = np.matmul(train_proc, train_align)
        train_transforms.append(train_transform)
        train_image.applyTransform(train_transform,
                                   ref_image.origin(), ref_image.dims(),
                                   ref_image.spacing(), ref_image.coordsys(),
                                   sw.InterpolationType.Linear, meshTransform=True)
    # Get bounding box using groomed meshes
    bounding_box = sw.MeshUtils.boundingBox(train_mesh_list).pad(10)
    # Crop images
    for train_image in train_image_list:
        train_image.crop(bounding_box)
    # Write images
    print("Writing groomed train images.")
    train_image_files = sw.utils.save_images(data_dir + 'train_images/', train_image_list,
                                             train_names, extension='nrrd', compressed=True, verbose=False)

    ######################################################################################
    print("\nStep 6. Augment data")
    """
    Step 6: Create additional training data via augmentation
    Parameters:
    - num_samples is how many samples to generate 
    - num_dim is the number of PCA scores to use
    - percent_dim what percent of variability to retain (used if num_dim is 0)
    - sampler is the distribution to use for sampling. Can be gaussian, mixture, or kde
    """
    num_samples = 2961
    num_dim = 0
    percent_variability = 0.95
    sampler = "kde"
    if args.tiny_test:
        num_samples = 2
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

    ######################################################################################
    print("\nStep 7. Find Test and Validation Transforms and Groom Images")
    """
    Step 7: Find test and validation transforms and images
    1. Find reflection
    2. Translate to have the same center as ref image
    3. Crop with large bounding box and translate
    4. Crop with medium bounding box and find rigid transform
    5. Apply full cropping and find similarity transform
    """
    # Get reference image
    ref_image_file = data_dir + 'reference_image.nrrd'
    ref_image = sw.Image(ref_image_file)
    ref_center = ref_image.center()  # get center
    # Slightly cropped ref image
    large_bb = sw.PhysicalRegion(bounding_box.min, bounding_box.max).pad(80)
    large_cropped_ref_image_file = data_dir + 'large_cropped_reference_image.nrrd'
    large_cropped_ref_image = sw.Image(ref_image_file).crop(large_bb).write(large_cropped_ref_image_file)
    # Further cropped ref image
    medium_bb = sw.PhysicalRegion(bounding_box.min, bounding_box.max).pad(20)
    medium_cropped_ref_image_file = data_dir + 'medium_cropped_reference_image.nrrd'
    medium_cropped_ref_image = sw.Image(ref_image_file).crop(medium_bb).write(medium_cropped_ref_image_file)
    # Fully cropped ref image
    cropped_ref_image_file = data_dir + 'cropped_reference_image.nrrd'
    cropped_ref_image = sw.Image(ref_image_file).crop(bounding_box).write(cropped_ref_image_file)

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

        # 1. Apply reflection transform
        reflection = np.eye(4)  # Identity
        if ref_side in vt_name:
            reflection[0][0] = -1  # Reflect across X
            reflection[-1][0] = 2 * vt_image.center()[0]  # account for offset
        vt_image.applyTransform(reflection)
        transform = sw.utils.getVTKtransform(reflection)

        # 2. Translate to have ref center to make rigid registration easier
        translation = ref_center - vt_image.center()
        vt_image.setOrigin(vt_image.origin() + translation).write(vt_image_file)
        transform[:3, -1] += translation

        # 3. Translate with respect to slightly cropped ref
        vt_image = sw.Image(vt_image_file).crop(large_bb).write(vt_image_file)
        itk_translation_transform = DeepSSMUtils.get_image_registration_transform(large_cropped_ref_image_file,
                                                                                  vt_image_file,
                                                                                  transform_type='translation')
        # Apply transform
        vt_image.applyTransform(itk_translation_transform,
                                large_cropped_ref_image.origin(), large_cropped_ref_image.dims(),
                                large_cropped_ref_image.spacing(), large_cropped_ref_image.coordsys(),
                                sw.InterpolationType.Linear, meshTransform=False)
        vtk_translation_transform = sw.utils.getVTKtransform(itk_translation_transform)
        transform = np.matmul(vtk_translation_transform, transform)

        # 4. Crop with medium bounding box and find rigid transform
        vt_image.crop(medium_bb).write(vt_image_file)
        itk_rigid_transform = DeepSSMUtils.get_image_registration_transform(medium_cropped_ref_image_file,
                                                                            vt_image_file, transform_type='rigid')
        # Apply transform
        vt_image.applyTransform(itk_rigid_transform,
                                medium_cropped_ref_image.origin(), medium_cropped_ref_image.dims(),
                                medium_cropped_ref_image.spacing(), medium_cropped_ref_image.coordsys(),
                                sw.InterpolationType.Linear, meshTransform=False)
        vtk_rigid_transform = sw.utils.getVTKtransform(itk_rigid_transform)
        transform = np.matmul(vtk_rigid_transform, transform)

        # 5. Get similarity transform from image registration and apply
        vt_image.crop(bounding_box).write(vt_image_file)
        itk_similarity_transform = DeepSSMUtils.get_image_registration_transform(cropped_ref_image_file,
                                                                                 vt_image_file,
                                                                                 transform_type='similarity')
        vt_image.applyTransform(itk_similarity_transform,
                                cropped_ref_image.origin(), cropped_ref_image.dims(),
                                cropped_ref_image.spacing(), cropped_ref_image.coordsys(),
                                sw.InterpolationType.Linear, meshTransform=False)
        vtk_similarity_transform = sw.utils.getVTKtransform(itk_similarity_transform)
        transform = np.matmul(vtk_similarity_transform, transform)
        # Save transform
        val_test_transforms.append(transform)

    # split val and test groomed images and transforms
    val_image_files = val_test_image_files[:len(val_mesh_files)]
    val_transforms = val_test_transforms[:len(val_mesh_files)]
    test_image_files = val_test_image_files[len(val_mesh_files):]
    test_transforms = val_test_transforms[len(val_mesh_files):]
    print("Done.")

    ######################################################################################
    print("\nStep 8. Optimize Validation Particles with Fixed Domains")
    """
    Step 8: Optimize particles on validation and training meshes, keeping
    training particles fixed and initializing validation particles to mean
    For more information about optimization see:
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """
    # Get validation names
    val_names = []
    for val_mesh_filename in val_mesh_files:
        val_name = os.path.basename(val_mesh_filename).replace('.ply', '')
        val_names.append(val_name)

    # Get meanshape in world particles
    mean_shape = np.loadtxt(train_world_particles[0])
    for i in range(1, len(train_world_particles)):
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
        transform = [train_transforms[i].flatten()]
        subject.set_groomed_transforms(transform)
        subject.set_constraints_filenames(rel_plane_files)
        subject.set_local_particle_filenames([train_local_particles[i]])
        subject.set_world_particle_filenames([train_local_particles[i]])
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
        transform = [val_transforms[i].flatten()]
        subject.set_groomed_transforms(transform)
        subject.set_constraints_filenames(rel_plane_files)
        subject.set_local_particle_filenames(rel_particle_files)
        subject.set_world_particle_filenames(rel_particle_files)
        subject.set_extra_values({"fixed": "no"})
        subjects.append(subject)
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Update parameter dictionary from step 4
    parameter_dictionary["multiscale"] = 0
    parameter_dictionary["procrustes"] = 0
    parameter_dictionary["procrustes_interval"] = 0
    parameter_dictionary["procrustes_scaling"] = 0
    parameter_dictionary["narrow_band"] = 1e10
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
    optimize_cmd = ('shapeworks optimize --progress --name ' + spreadsheet_file).split()
    subprocess.check_call(optimize_cmd)

    print("To analyze validation shape model, call:")
    print(" ShapeWorksStudio " + spreadsheet_file)

    # Get validation world particle files
    val_world_particles = []
    for val_name in val_names:
        val_world_particles.append(data_dir + 'validation_particles/' + val_name + "_world.particles")

    ######################################################################################
    print("\nStep 9. Create PyTorch loaders from data.")
    """
    Step 9: Create PyTorch loaders from data
    Create training, validation, and test torch loaders
    If down_sample is true, model will train on images that are smaller by down_factor
    Hyper-parameter batch_size is for training
        Higher batch size will help speed up training but uses more cuda memory, 
        if you get a memory error try reducing the batch size
    """
    batch_size = 8
    loader_dir = output_directory + 'torch_loaders/'
    DeepSSMUtils.getTrainLoader(loader_dir, aug_data_csv, batch_size)
    DeepSSMUtils.getValidationLoader(loader_dir, val_image_files, val_world_particles)
    DeepSSMUtils.getTestLoader(loader_dir, test_image_files)

    ######################################################################################
    print("\nStep 10. Train DeepSSM model.")
    """
    Step 10: Train DeepSSM model on training loader
    This requires creating a json config file of model parameters
    """
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
            "epochs": 10,
            "learning_rate": 0.001,
            "decay_lr": {
                "enabled": True,
                "type": "CosineAnnealing",
                "parameters": {
                    "T_max": 10,
                    "eta_min": 0,
                    "step_size": 1,
                    "gamma": 0.99
                }
            },
            "val_freq": 1
        },
        "fine_tune": {
            "enabled": False,
            "epochs": 10,
            "loss": "MSE",
            "learning_rate": 0.001,
            "decay_lr": True,
            "val_freq": 1
        },
        "use_best_model": True,
        "tl_net": {
            "enabled": False,
            "ae_epochs": 100,
            "tf_epochs": 100,
            "joint_epochs": 25,
            "alpha": 1,
            "a_ae": 10,
            "c_ae": 1.32,
            "a_lat": 10,
            "c_lat": 6.3
        }
    }
    if args.tiny_test:
        model_parameters["trainer"]["epochs"] = 1
    # Save config file    
    config_file = output_directory + model_name + ".json"
    with open(config_file, "w") as outfile:
        json.dump(model_parameters, outfile, indent=2)
    # Train
    DeepSSMUtils.trainDeepSSM(config_file)

    ######################################################################################
    print("\nStep 11. Predict validation particles with trained DeepSSM and analyze accuracy.")
    """
    Step 11: Validation analysis
    Use trained DeepSSM model to predict world validation particles
    and apply inverse transforms to get local predictions
    Analyze accuracy
    """
    val_out_dir = output_directory + model_name + '/validation_predictions/'
    predicted_val_world_particles = DeepSSMUtils.testDeepSSM(config_file, loader='validation')
    print("Validation world predictions saved.")
    # Generate local predictions
    local_val_prediction_dir = val_out_dir + 'local_predictions/'
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
    - MSE between true and predicted world particles
    - Surface to surface distance between true mesh and mesh generated from predicted local particles
    '''
    mean_MSE, std_MSE = DeepSSMUtils.analyzeMSE(predicted_val_world_particles, val_world_particles)
    print("Validation world particle MSE: " + str(mean_MSE) + " +- " + str(std_MSE))
    template_mesh = train_mesh_files[ref_index]
    template_particles = train_local_particles[ref_index].replace("./", data_dir)
    # Get distance between clipped true and predicted meshes
    mean_dist = DeepSSMUtils.analyzeMeshDistance(predicted_val_local_particles, val_mesh_files,
                                                 template_particles, template_mesh, val_out_dir,
                                                 planes=val_planes)
    print("Validation mean mesh surface-to-surface distance: " + str(mean_dist))

    # If tiny test or verify, check results and exit
    check_results(args, mean_dist)

    ######################################################################################
    print("\nStep 12. Predict test particles with trained DeepSSM and analyze accuracy.")
    """
    Step 12: Test analysis
    Use trained DeepSSM model to predict world validation particles
    and apply inverse transforms to get local predcitions
    Analyze accuracy
    """
    test_out_dir = output_directory + model_name + '/test_predictions/'
    predicted_test_world_particles = DeepSSMUtils.testDeepSSM(config_file, loader='test')
    print("Test world predictions saved.")
    # Generate local predictions
    local_test_prediction_dir = test_out_dir + 'local_predictions/'
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
    clipped true mesh and clipped mesh generated from predicted local particles
    '''
    mean_dist = DeepSSMUtils.analyzeMeshDistance(predicted_test_local_particles, test_mesh_files,
                                                 template_particles, template_mesh, test_out_dir,
                                                 planes=test_planes)
    print("Test mean mesh surface-to-surface distance: " + str(mean_dist))


# Verification
def check_results(args, mean_dist):
    if args.tiny_test:
        print("\nVerifying use case results.")
        if not math.isclose(mean_dist, 10, rel_tol=1):
            print("Test failed.")
            exit(-1)
        print("Done with test, verification succeeded.")
        exit(0)
    else:
        return
