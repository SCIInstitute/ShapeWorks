"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with Progressive DeepSSM
====================================================================
"""
import os
import glob
import platform
import torch
import random
import shutil
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

    model_name = "femur_deepssm"
    dataset_name = "progressive_deep_ssm_femur"
    output_directory = os.getcwd() + "/Output/deep_ssm/"

    if args.clean:
        print("Cleaning output directory: " + output_directory)
        shutil.rmtree(output_directory)
        if args.tiny_test:
            os.remove("Output/deep_ssm_femur_tiny_test.downloaded")
        else:
            os.remove("Output/deep_ssm_femur.downloaded")
        

    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    status_dir = output_directory + "status/"
    if args.tiny_test:
        status_dir = output_directory + "tiny_test_status/"
    if not os.path.exists(status_dir):
        os.makedirs(status_dir)

    data_dir = output_directory + 'data/'
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)

    ######################################################################################
    # Step 1. Acquire Data
    ######################################################################################
    if not os.path.exists(status_dir + "step_1.txt"):
        print("\nStep 1. Acquire Data")
        """
        Step 1: ACQUIRE DATA
        We define dataset_name which determines which dataset to download from
        the portal and the directory to save output from the use case in.
        This data is comprised of femur meshes and corresponding hip CT scans.
        """

        if platform.system() == "Darwin":
            # On MacOS, CPU PyTorch is hanging with parallel
            os.environ['OMP_NUM_THREADS'] = "1"
        # If running a tiny_test, then download subset of the data
        if args.tiny_test:
            dataset_name = "deep_ssm_femur_tiny_test"
            sw.download_dataset(dataset_name, output_directory, args.clean)
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

        # Construct ShapeWorks Project
        ref_side = "L"  # chosen so reflection happens in tiny test

        # Create Subjects
        subjects = []
        for i in range(len(mesh_files)):
            subject = sw.Subject()
            subject.set_original_filenames([mesh_files[i]])
            subject.set_constraints_filenames([plane_files[i]])

            identifier = os.path.basename(mesh_files[i]).split("_")[0]
            corresponding_image_file = ""
            for index in range(len(image_files)):
                if identifier in image_files[index]:
                    corresponding_image_file = image_files[index]
                    break
            if corresponding_image_file == "":
                raise Exception("No corresponding image file found for mesh: " + mesh_files[i])
            subject.set_feature_filenames({"CT": corresponding_image_file})

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

        open(status_dir + "step_1.txt", 'w').close()
    else:
        # Load project spreadsheet
        spreadsheet_file = data_dir + "deepssm_project.xlsx"
        print("\nLoading project spreadsheet: " + spreadsheet_file)
        project = sw.Project()
        project.load(spreadsheet_file)

    print(f"Using project: {spreadsheet_file}")
    # Now that the project is ready, set up some paths
    project_path = project.get_project_path() + "/"
    os.chdir(project_path)
    deepssm_dir = DeepSSMUtils.get_deepssm_dir(project)
    aug_dir = deepssm_dir + "augmentation/"

    ######################################################################################
    # Step 2. Define Split
    ######################################################################################
    if not os.path.exists(status_dir + "step_2.txt"):
        print("\nStep 2. Define Split")
        if args.tiny_test:
            DeepSSMUtils.create_split(project, 60, 20, 20)
        else:
            DeepSSMUtils.create_split(project, 80, 10, 10)
        project.save(spreadsheet_file)
        open(status_dir + "step_2.txt", 'w').close()
    
    if not os.path.exists(status_dir + "step_3.txt"):
        print("\nStep 3. Groom Training Shapes")

        params = project.get_parameters("groom")
        params.set("alignment_method", "Iterative Closest Point")
        params.set("alignment_reference", "32")
        params.set("alignment_subset_size", "20")
        params.set("remesh", "0")
        params.set("reflect", "1")
        params.set("reflect_column", "side")
        params.set("reflect_choice", "L")
        params.set("reflect_axis", "X")
        project.set_parameters("groom", params)

        sw_message("Grooming training data...")
        DeepSSMUtils.groom_training_shapes(project)
        project.save(spreadsheet_file)

        reference_index = sw.utils.get_reference_index(project)
        print("Reference index: " + str(reference_index))
        # print reference mesh name
        print("Reference mesh: " + project_path + project.get_subjects()[reference_index].get_original_filenames()[0])

        open(status_dir + "step_3.txt", 'w').close()

    ######################################################################################
    # Step 4. Optimize Training Particles
    ######################################################################################
    if not os.path.exists(status_dir + "step_4.txt"):
        print("\nStep 4. Optimize Training Particles")

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
            "multiscale": 1,
            "multiscale_particles": 64
        }
        # If running a tiny test, reduce some parameters
        if args.tiny_test:
            parameter_dictionary["number_of_particles"] = 64
            parameter_dictionary["optimization_iterations"] = 25
        

        for key in parameter_dictionary:
            parameters.set(key, sw.Variant([parameter_dictionary[key]]))
        project.set_parameters("optimize", parameters)
        # spreadsheet_file = data_dir + "train.xlsx"
        project.save(spreadsheet_file)

        sw_message("Optimize training particles...")
        DeepSSMUtils.optimize_training_particles(project)

        project.save(spreadsheet_file)
        open(status_dir + "step_4.txt", 'w').close()


    ######################################################################################
    # Step 5. Groom Training Images
    ######################################################################################
    if not os.path.exists(status_dir + "step_5.txt"):
        print("\nStep 5. Groom Training Images")
        DeepSSMUtils.groom_training_images(project)
        open(status_dir + "step_5.txt", 'w').close()