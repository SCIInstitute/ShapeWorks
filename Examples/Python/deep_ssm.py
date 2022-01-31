# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks DeepSSM
====================================================================
"""
import os
import glob
import shapeworks as sw
import platform
import torch
import random
import math
import numpy as np
import subprocess
import itk
import scipy
# import DataAugmentationUtils
# import DeepSSMUtils
# import json
torch.multiprocessing.set_sharing_strategy('file_system')
random.seed(1)

def Run_Pipeline(args):

    print("\nStep 1. Extract Data")
    """
    Step 1: EXTRACT DATA
    We define dataset_name which determines which dataset to download from
    the portal and the directory to save output from the use case in.
    This data is comprised of femur meshes and corresponding hip CT scans.
    """
    dataset_name = "femur-v1"
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
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))
        image_files = sorted(glob.glob(output_directory + 
                            dataset_name + "/images/*.nrrd"))

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


    print("\nStep 3. Find Training Mesh Transforms")
    """
    Step 3: Train Mesh Grooming 
    1. Find reflection tansfrom
    2. Select reference mesh
    3. Find rigid alignment transform
    """
    # Make folder to save data
    data_dir = output_directory + 'data/'
    if not os.path.exists(data_dir):
         os.makedirs(data_dir)
    # To begin grooming, we loop over the files and load the meshes
    ref_side = "L" # chosen arbitrarily
    train_mesh_list = []
    train_names = []
    train_reflections = [] 
    print("Loading meshes and finding reflection transforms...")
    for train_mesh_file in train_mesh_files:
        # Get name
        train_name = os.path.basename(train_mesh_file).replace('.ply', '')
        train_names.append(train_name)
        # Load mesh
        train_mesh = sw.Mesh(train_mesh_file)
        train_mesh_list.append(train_mesh)
        # Find reflection transform if needed
        train_reflection = np.eye(4) # Identity
        if ref_side in train_name:
            train_reflection[0][0] = -1 # Reflect across X
        train_mesh.applyTransform(train_reflection) # apply for rigid alignment
        train_reflections.append(train_reflection)
    # Select reference mesh
    print("Finding reference mesh...")
    ref_index = sw.find_reference_mesh_index(train_mesh_list)
    ref_mesh = train_mesh_list[ref_index].copy().write(data_dir + 'reference_mesh.vtk')
    print("Reference found: " + train_names[ref_index])
    # Find rigid transforms to align each mesh to the reference 
    train_rigid_transforms = [] 
    print("Finding rigid alignment transforms...")
    for train_mesh, train_name in zip(train_mesh_list, train_names):
        # compute rigid transformation
        train_rigid_transform = train_mesh.createTransform(ref_mesh, sw.Mesh.AlignmentType.Rigid, 100)
        train_mesh.applyTransform(train_rigid_transform) # apply for finding bounding box
        train_rigid_transforms.append(train_rigid_transform)
    print("Write transforms...")
    # Make directory for train transform
    train_transform_dir = data_dir + 'train_transforms/'
    if not os.path.exists(train_transform_dir):
        os.makedirs(train_transform_dir)
    # Combine transforms to pass to optimizer
    train_transforms = []
    for train_name, train_reflection, train_rigid_transform in zip(train_names, train_reflections, train_rigid_transforms):
        train_transform = np.matmul(train_rigid_transform, train_reflection)
        train_transforms.append(train_transform)
        np.save(train_transform_dir, train_transform)
    print("Training mesh transforms found.")


    print("\nStep 4. Groom Training Images")
    """
    Step 4: Load training images and apply transforms
    """
    train_image_list = []
    print("Loading images and reflecting...")
    for train_name, train_reflection in zip(train_names, train_reflections):
        ID = train_name.split("_")[0]
        # Get corresponding image
        for index in range(len(image_files)):
            if ID in image_files[index]:
                corresponding_image_file = image_files[index]
                break
        train_image = sw.Image(corresponding_image_file)
        train_image_list.append(train_image)
        # Apply reflection transform 
        train_image.applyTransform(train_reflection)
    # Get reference image
    print("Getting reference image.")
    ref_image = train_image_list[ref_index].copy()
    ref_image.resample([1,1,1], sw.InterpolationType.Linear)
    ref_image.write(data_dir + 'reference_image.nrrd')
    # Applyinfg rigid transform
    print("Applying rigid transforms...")
    for train_image, train_rigid_transform in zip(train_image_list, train_rigid_transforms):
        # Align image
        train_image.applyTransform(train_rigid_transform,
                             ref_image.origin(),  ref_image.dims(),
                             ref_image.spacing(), ref_image.coordsys(),
                             sw.InterpolationType.Linear, meshTransform=True)
    # Get bounding box
    print("Getting bounding box.")
    bounding_box = sw.MeshUtils.boundingBox(train_mesh_list)
    bounding_box.pad(10)
    # bounding_box.write(data_dir + "bounding_box.txt")
    print("Applying cropping...")
    for train_image in train_image_list:
        # Crop image
        train_image.crop(bounding_box)
    # Write images
    print("Writing groomed train images.")
    train_image_files = sw.utils.save_images(data_dir + 'train_images/', train_image_list,
                    train_names, extension='nrrd', compressed=True, verbose=False)


    print("\nStep 5. Find Test and Validation Transforms and Groom Images")
    """
    Step 5: Find test and validation transforms and images
    1. Find reflection
    2. Find alignment transform using rigid image registration
    3. Apply alignment to image
    4. Crop image
    """
    # Helper function - Works best if moving and fixed images have been centered
    def rigid_image_registration_transform(fixed_image_file, moving_image_file, out_image_file):
        fixed_image = itk.imread(fixed_image_file, itk.F)
        moving_image = itk.imread(moving_image_file, itk.F)
        # Import Default Parameter Map
        parameter_object = itk.ParameterObject.New()
        default_rigid_parameter_map = parameter_object.GetDefaultParameterMap('rigid')
        parameter_object.AddParameterMap(default_rigid_parameter_map)
        # Call registration function
        result_image, result_transform_parameters = itk.elastix_registration_method(
            fixed_image, moving_image, parameter_object=parameter_object)
        itk.imwrite(result_image, out_image_file)
        # Get transform matrix - can't find a way to do it without writing it
        result_transform_parameters.WriteParameterFile("temp.txt")
        with open("temp.txt") as f:
            lines = f.readlines()
        transform_params = lines[-2].replace(")\n","").split(" ")[1:]
        transform_params = np.array(transform_params).astype(np.float)
        thetas = transform_params[:3]
        rotation_matrix = scipy.spatial.transform.Rotation.from_euler('XYZ',thetas).as_matrix()
        translation = transform_params[3:]
        itk_transform_matrix = np.eye(4)
        itk_transform_matrix[:3,:3] = rotation_matrix
        itk_transform_matrix[-1,:3] = translation
        vtk_tranform_matrix = sw.utils.getVTKtransform(itk_transform_matrix)
        os.remove("temp.txt")
        return vtk_tranform_matrix
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
    val_test_transforms_dir = data_dir + 'val_and_test_transforms/'
    if not os.path.exists(val_test_transforms_dir):
    	os.makedirs(val_test_transforms_dir)
    # Combine mesh files
    val_test_mesh_files = val_mesh_files + test_mesh_files
    # Loop over
    print("Grooming validation and test images...")
    val_test_images = []
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
        val_test_images.append(vt_image)
        # Apply reflection transform 
        reflection = np.eye(4) # Identity
        if ref_side in vt_name:
            reflection[0][0] = -1 # Reflect across X
        vt_image.applyTransform(reflection)
        # Translate to make rigid registration easier
        translation = ref_center - vt_image.center()
        vt_image.setOrigin(vt_image.origin() + translation)
        vt_image_file = val_test_images_dir + vt_name + ".nrrd"
        vt_image.write(vt_image_file)
        # Get rigid transform from image registration and apply
        # Roughly align, roughly crop, then realign
        print("Finding rigid transform for: " + vt_name)
        rigid_transform1 = rigid_image_registration_transform(ref_image_file, vt_image_file, vt_image_file)
        # vt_image = sw.Image(vt_image_file).crop(padded_bb).write(vt_image_file)
        # rigid_transform2 = rigid_image_registration_transform(cropped_ref_image_file, vt_image_file, vt_image_file)
        # Crop
        sw.Image(vt_image_file).crop(bounding_box).write(vt_image_file)
        # Get combined transform and write
        translation_marix = np.eye(4)
        translation_marix[:3,-1] = translation
        # transform = np.matmul(translation_marix, reflection)
        # transform = np.matmul(rigid_transform1, transform)
        # transform = np.matmul(rigid_transform2, transform)
        transform = np.matmul(reflection, translation_marix)
        transform = np.matmul(transform, rigid_transform1)
        transform = np.matmul(transform, rigid_transform2)
        val_test_transforms.append(transform)
        np.save(val_test_transforms_dir, transform)

    # Debug
    groomed_meshes = []
    names = []
    for mesh_file, transform in zip(train_mesh_files, train_transforms):
    	names.append(mesh_file.split('/')[-1])
    	groomed_meshes.append(sw.Mesh(mesh_file).applyTransform(transform))
    m_files = sw.utils.save_meshes(data_dir + 'train_meshes/', groomed_meshes, names)
    groomed_meshes = []
    names = []
    for mesh_file, transform in zip(val_test_mesh_files, val_test_transforms):
    	names.append(mesh_file.split('/')[-1])
    	groomed_meshes.append(sw.Mesh(mesh_file).applyTransform(transform))
    m_files = sw.utils.save_meshes(data_dir + 'val_test_meshes/', groomed_meshes, names)


    # print("\nStep 6. Optimize Training Particles")
    # """
    # Step 6: OPTIMIZE - Particle based optimization on training meshes
    # Local particles will be in alignment with original images and meshes
    # World particles would be in alignment with groomed images from the previous
    # step if Procrustes was set to off
    # Visit this link for more information about optimization: 
    # http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    # """
    # # Create spreadsheet
    # subjects = []
    # for i in range(len(train_mesh_list)):
    #     subject = sw.Subject()
    #     subject.set_number_of_domains(1)
    #     subject.set_segmentation_filenames([train_mesh_files[i]])
    #     subject.set_groomed_filenames([train_mesh_files[i]])
    #     subject.set_groomed_transforms([train_transforms[i].flatten()])
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
    # spreadsheet_file = data_dir + "train_optimize.xlsx"
    # project.save(spreadsheet_file)
    # # Optimize 
    # optimizeCmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    # subprocess.check_call(optimizeCmd)
    # # Analyze
    # AnalysisCmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    # subprocess.check_call(AnalysisCmd)
    # # Get lists of particle files
    # train_particle_dir = data_dir+"train_optimize_particles/"
    # train_world_particle_files = []
    # train_local_particle_files = []
    # for file in sorted(os.listdir(train_particle_dir)):
    #     if "local.particles" in file:
    #         train_local_particle_files.append(train_particle_dir + file)
    #     elif "world.particles" in file:
    #         train_world_particle_files.append(train_particle_dir + file)


    print("\nStep 7. Optimize Validation and Test Particles Using Fixed Domain")
    """
    TODO: Make this fixed domain!!
    Step 7: FIXED DOMAIN OPTIMIZE - Particle based optimization for validation and test using fixed domains
    Particles for training meshes will not be updated
    Local particles will be in alignment with original images and meshes
    World particles would be in alignment with groomed images from the previous step if Procrustes was set to off
    Visit this link for more information about optimization: 
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """
    # Create spreadsheet
    subjects = []
    for i in range(len(train_mesh_files)):
        subject = sw.Subject()
        subject.set_number_of_domains(1)
        subject.set_segmentation_filenames([train_mesh_files[i]])
        subject.set_groomed_filenames([train_mesh_files[i]])
        subject.set_groomed_transforms([train_transforms[i].flatten()])
        subjects.append(subject)
    for i in range(len(val_test_mesh_files)):
        subject = sw.Subject()
        subject.set_number_of_domains(1)
        subject.set_segmentation_filenames([val_test_mesh_files[i]])
        subject.set_groomed_filenames([val_test_mesh_files[i]])
        subject.set_groomed_transforms([val_test_transforms[i].flatten()])
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
    # Set params and save spreadsheet
    for key in parameter_dictionary:
        parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    parameters.set("domain_type",sw.Variant('mesh'))
    project.set_parameters("optimze",parameters)
    spreadsheet_file = data_dir + "val_test_optimize.xlsx"
    project.save(spreadsheet_file)
    # Optimize 
    optimizeCmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    subprocess.check_call(optimizeCmd)
    # Analyze
    AnalysisCmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(AnalysisCmd)