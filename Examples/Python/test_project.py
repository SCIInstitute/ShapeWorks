# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================
This example is set to serve as a test case for new ShapeWorks users, and each
step is explained in the shapeworks including the pre-processing, the 
optimization and, the post ShapeWorks visualization.

First import the necessary modules
"""
import os
import glob
import shapeworks as sw
import numpy as np
import OptimizeUtils
import AnalyzeUtils
import subprocess
import shutil
def Run_Pipeline(args):

    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "ellipsoid_1mode"
    output_directory = "Output/ellipsoid_proj/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        sw.data.download_subset(
            args.use_case, dataset_name, output_directory)
        file_list = sorted(glob.glob(output_directory +
                                     dataset_name + "/segmentations/*.nrrd"))[:3]
    # Else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        file_list = sorted(glob.glob(output_directory +
                                     dataset_name + "/segmentations/*.nrrd"))

        # Select representative data if using subsample
        if args.use_subsample:
            inputImages = [sw.Image(filename) for filename in file_list]
            sample_idx = sw.data.sample_images(
                inputImages, int(args.num_subsample))
            file_list = [file_list[i] for i in sample_idx]

    # If skipping grooming, use the pregroomed distance transforms from the portal
    if args.skip_grooming:
        print("Skipping grooming.")
        dt_directory = output_directory + dataset_name + '/groomed/distance_transforms/'
        indices = []
        if args.tiny_test:
            indices = [0, 1, 2]
        elif args.use_subsample:
            indices = sample_idx
        dt_files = sw.data.get_file_list(
            dt_directory, ending=".nrrd", indices=indices)

    # Else groom the segmentations and get distance transforms for optimization
    else:
        print("\nStep 2. Groom - Data Pre-processing\n")
       
        # Create a directory for groomed output
        groom_dir = output_directory + 'groomed/'
        if not os.path.exists(groom_dir):
            os.makedirs(groom_dir)

        """
        First, we need to loop over the shape segmentation files and load the segmentations
        """
        # list of shape segmentations
        shape_seg_list = []
        # list of shape names (shape files prefixes) to be used for saving outputs
        shape_names = []
        for shape_filename in file_list:
            print('Loading: ' + shape_filename)
            # get current shape name
            shape_names.append(shape_filename.split('/')
                               [-1].replace('.nrrd', ''))
            # load segmentation
            shape_seg = sw.Image(shape_filename)
            # append to the shape list
            shape_seg_list.append(shape_seg)




        #Select a reference
        ref_index = sw.find_reference_image_index(shape_seg_list)
        # Make a copy of the reference segmentation
        ref_seg = shape_seg_list[ref_index].write(groom_dir + 'reference.nrrd')
        ref_name = shape_names[ref_index]
        print("Reference found: " + ref_name)

        # Set the alignment parameters
        iso_value = 0.5
        icp_iterations = 100
        Rigid_transforms = []
        idx = 0
        
        #Now loop through all the segmentations and calculate ICP transform
        for shape_seg, shape_name in zip(shape_seg_list, shape_names):
            print('Finding alignment transform from ' + shape_name + ' to ' + ref_name)
            
            
            rigidTransform = shape_seg.createRigidRegistrationTransform(
                ref_seg, iso_value, icp_iterations)
            # rigidTransform[:,-1] = center_transform[idx][:,-1]
            # idx+=1
            rigidTransform = sw.utils.getVTKtransform(rigidTransform)
            shape_center = ref_seg.centerOfMass()
            # # input(rigidTransform)
            transform = np.eye(4)

            Rigid_transforms.append(rigidTransform)
            # input(rigidTransform)
            print(rigidTransform)
        """
        Now we can loop over the segmentations and apply the initial grooming steps to themm
        """
        # list of shape segmentations
        shape_seg_list = []
        # list of shape names (shape files prefixes) to be used for saving outputs
        shape_names = []
        for shape_filename in file_list:
            print('Loading: ' + shape_filename)
            # get current shape name
            shape_names.append(shape_filename.split('/')
                               [-1].replace('.nrrd', ''))
            # load segmentation
            shape_seg = sw.Image(shape_filename)
            # append to the shape list
            shape_seg_list.append(shape_seg)

        center_transform = []
        i = 0 
        for shape_seg, shape_name in zip(shape_seg_list, shape_names):
            # parameters for padding
            padding_size = 10  # number of voxels to pad for each dimension
            padding_value = 0  # the constant value used to pad the segmentations
            # pad the image 
            shape_seg.pad(padding_size,padding_value)

            # antialias for 30 iterations
            antialias_iterations = 30
            shape_seg.antialias(antialias_iterations)
            shape_seg.binarize()
            # Define distance transform parameters
            iso_value = 0
            sigma = 1.3
            #Converting segmentations to smooth signed distance transforms.
            shape_seg.antialias(antialias_iterations).computeDT(
                iso_value).gaussianBlur(sigma)

        #Save distance transforms
        dt_files = sw.utils.save_images(groom_dir + 'distance_transforms/', shape_seg_list,
                                        shape_names, extension='nrrd', compressed=True, verbose=True)


       

        subjects = []
        number_domains = 1
        seg_dir = output_directory+"ellipsoid_1mode/segmentations/"
        for i in range(len(shape_seg_list)):
            print(shape_names[i])
            subject = sw.Subject()
            subject.set_number_of_domains(number_domains)
            # subject.set_segmentation_filenames([groomed_seg_files[i]])
            subject.set_segmentation_filenames([seg_dir+shape_names[i]+".nrrd"])
            subject.set_groomed_filenames([dt_files[i]])
            transform = Rigid_transforms[i]
            transforms = [ transform.flatten() ]
            subject.set_groomed_transforms(transforms)
            print(subject.get_groomed_transforms())
            subjects.append(subject)

        project = sw.Project()
        project.set_subjects(subjects)
        parameters = sw.Parameters()

        parameter_dictionary = {
        "number_of_particles": 128,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 1000,
        "keep_checkpoints": 0,
        "iterations_per_split": 1000,
        "optimization_iterations": 1000,
        "starting_regularization": 10,
        "ending_regularization": 1,
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
        "narrow_band":4,
        "relative_weighting": 1,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0
        } 

        for key in parameter_dictionary:
            parameters.set(key,sw.Variant([parameter_dictionary[key]]))
        parameters.set("domain_type",sw.Variant('image'))
        project.set_parameters("optimze",parameters)
        project.set_filename("test_proj_parm.xlsx")
        project.save("test_proj_parm.xlsx")

        opt = sw.Optimize()
        opt.SetUpOptimize(project)
        opt.Run()
        project.save("test_proj_parm.xlsx")

        AnalysisCmd = 'ShapeWorksStudio test_proj_parm.xlsx'.split()
        subprocess.check_call(AnalysisCmd)

