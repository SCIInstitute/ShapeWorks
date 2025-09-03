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
    dataset_name = "deep_ssm_femur"
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

    ######################################################################################
    # Step 3. Groom Training Shapes
    ######################################################################################
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
        }
        # If running a tiny test, reduce some parameters
        if args.tiny_test:
            parameter_dictionary["number_of_particles"] = 64
            parameter_dictionary["optimization_iterations"] = 25
        # Run multi-scale optimization unless single scale is specified
        if not args.use_single_scale:
            parameter_dictionary["multiscale"] = 1
            parameter_dictionary["multiscale_particles"] = 64

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

    ######################################################################################
    # Step 6. Augment data
    ######################################################################################
    if not os.path.exists(status_dir + "step_6.txt"):
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

        embedded_dim = DeepSSMUtils.run_data_augmentation(project, num_samples, num_dim, percent_variability, sampler,
                                                          mixture_num=0, processes=1)

        print("Dimensions retained: " + str(embedded_dim))
        aug_data_csv = aug_dir + "TotalData.csv"

        if not args.tiny_test and not args.verify:
            DataAugmentationUtils.visualizeAugmentation(aug_data_csv, "violin")

        open(status_dir + "step_6.txt", 'w').close()

    ######################################################################################
    # Step 7. Groom Test and Validation Images
    ######################################################################################
    if not os.path.exists(status_dir + "step_7.txt"):
        print("\nStep 7. Groom Test and Validation Images")

        val_indices = DeepSSMUtils.get_split_indices(project, "val")
        test_indices = DeepSSMUtils.get_split_indices(project, "test")
        val_test_indices = val_indices + test_indices

        DeepSSMUtils.groom_val_test_images(project, val_test_indices)
        project.save(spreadsheet_file)
        open(status_dir + "step_7.txt", 'w').close()

    ######################################################################################
    # Step 8. Optimize Validation Particles with Fixed Domains
    ######################################################################################
    if not os.path.exists(status_dir + "step_8.txt"):
        DeepSSMUtils.prep_project_for_val_particles(project)

        # update parameters
        params = project.get_parameters("optimize")
        params.set("multiscale", "0")
        params.set("procrustes", "0")
        params.set("procrustes_interval", "0")
        params.set("procrustes_scaling", "0")
        params.set("narrow_band", "1e10")

        project.set_parameters("optimize", params)

        sw_message("Grooming validation shapes...")
        DeepSSMUtils.groom_validation_shapes(project)

        sw_message("Optimize validation particles...")
        project.save(spreadsheet_file)
        optimize = sw.Optimize()
        optimize.SetUpOptimize(project)
        optimize.Run()
        project.save(spreadsheet_file)
        open(status_dir + "step_8.txt", 'w').close()

    ######################################################################################
    # Step 9. Create PyTorch loaders from data.
    ######################################################################################
    if not os.path.exists(status_dir + "step_9.txt"):
        print("\nStep 9. Create PyTorch loaders from data.")

        """
        Create training, validation, and test torch loaders
        If down_sample is true, model will train on images that are smaller by down_factor
        Hyper-parameter batch_size is for training
            Higher batch size will help speed up training but uses more cuda memory, 
            if you get a memory error try reducing the batch size
        """

        batch_size = 8
        DeepSSMUtils.prepare_data_loaders(project, batch_size)
        open(status_dir + "step_9.txt", 'w').close()

    ######################################################################################
    # Step 10. Train DeepSSM model
    ######################################################################################
    config_file = output_directory + model_name + ".json"
    if not os.path.exists(status_dir + "step_10.txt"):
        print("\nStep 10. Train DeepSSM model.")
        """
        Step 10: Train DeepSSM model on training loader
        This requires creating a json config file of model parameters
        """
        loader_dir = deepssm_dir + 'torch_loaders/'

        # load embedded_dim from file
        with open(deepssm_dir + "embedded_dim.txt", "r") as f:
            embedded_dim = f.read()

        # Define model parameters
        model_parameters = {
            "model_name": model_name,
            "num_latent_dim": int(embedded_dim),
            "paths": {
                "out_dir": deepssm_dir,
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
        with open(config_file, "w") as outfile:
            json.dump(model_parameters, outfile, indent=2)
        # Train
        DeepSSMUtils.trainDeepSSM(project, config_file)
        open(status_dir + "step_10.txt", 'w').close()

    ######################################################################################
    # Step 11. Predict validation particles with trained DeepSSM and analyze accuracy.
    ######################################################################################
    if not os.path.exists(status_dir + "step_11.txt"):
        print("\nStep 11. Predict validation particles with trained DeepSSM and analyze accuracy.")
        """
        Step 11: Validation analysis
        Use trained DeepSSM model to predict world validation particles
        and apply inverse transforms to get local predictions
        Analyze accuracy
        """

        """ Predict validation particles and analyze accuracy """

        subjects = project.get_subjects()
        val_transforms = []
        val_planes = []
        val_world_particles = []
        val_mesh_files = []
        val_indices = DeepSSMUtils.get_split_indices(project, "val")
        for index in val_indices:
            alignment = subjects[index].get_groomed_transforms()[0]
            alignment = np.array(alignment)
            alignment = alignment.reshape(4, 4)
            val_transforms.append(alignment)
            plane_file = project_path + subjects[index].get_constraints_filenames()[0]
            with open(plane_file) as json_file:
                val_planes.append(json.load(json_file)['planes'][0]['points'])
            val_world_particles.append(project_path + subjects[index].get_world_particle_filenames()[0])
            val_mesh_files.append(project_path + subjects[index].get_groomed_filenames()[0])

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
            local_particle_file = particle_file.replace("world_predictions/", "local_predictions/")
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
        reference_index = sw.utils.get_reference_index(project)
        template_mesh = project_path + subjects[reference_index].get_groomed_filenames()[0]
        template_particles = project_path + subjects[reference_index].get_local_particle_filenames()[0]
        # Get distance between clipped true and predicted meshes
        mean_dist = DeepSSMUtils.analyzeMeshDistance(predicted_val_local_particles, val_mesh_files,
                                                     template_particles, template_mesh, val_out_dir,
                                                     planes=val_planes)

        print("Validation mean mesh surface-to-surface distance: " + str(mean_dist))

        open(status_dir + "step_11.txt", 'w').close()

    ######################################################################################
    # Step 12. Predict test particles with trained DeepSSM and analyze accuracy.
    ######################################################################################
    if not os.path.exists(status_dir + "step_12.txt"):
        print("\nStep 12. Predict test particles with trained DeepSSM and analyze accuracy.")
        """
        Step 12: Test analysis
        Use trained DeepSSM model to predict world validation particles
        and apply inverse transforms to get local predictions
        Analyze accuracy
        """

        subjects = project.get_subjects()
        test_transforms = []
        test_mesh_files = []
        test_planes = []
        test_indices = DeepSSMUtils.get_split_indices(project, "test")
        for index in test_indices:
            alignment = subjects[index].get_extra_values()["registration_transform"]
            # parse alignment from space separated string to 4x4 matrix
            alignment = alignment.split()
            alignment = [float(i) for i in alignment]
            alignment = np.array(alignment)
            alignment = alignment.reshape(4, 4)
            test_transforms.append(alignment)
            test_mesh_files.append(project_path + subjects[index].get_original_filenames()[0])
            plane_file = project_path + subjects[index].get_constraints_filenames()[0]
            with open(plane_file) as json_file:
                test_planes.append(json.load(json_file)['planes'][0]['points'])

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
            local_particle_file = particle_file.replace("world_predictions/", "local_predictions/")
            local_particles = sw.utils.transformParticles(particles, transform, inverse=True)
            np.savetxt(local_particle_file, local_particles)
            predicted_test_local_particles.append(local_particle_file)
        print("Test local predictions written to: " + local_test_prediction_dir)
        '''
        Analyze test accuracy in terms of surface to surface distance between 
        clipped true mesh and clipped mesh generated from predicted local particles
        '''

        reference_index = sw.utils.get_reference_index(project)
        template_mesh = project_path + subjects[reference_index].get_groomed_filenames()[0]
        template_particles = project_path + subjects[reference_index].get_local_particle_filenames()[0]

        mean_dist = DeepSSMUtils.analyzeMeshDistance(predicted_test_local_particles, test_mesh_files,
                                                     template_particles, template_mesh, test_out_dir,
                                                     planes=test_planes)
        print("Test mean mesh surface-to-surface distance: " + str(mean_dist))

        DeepSSMUtils.process_test_predictions(project, config_file)
        
        # If tiny test or verify, check results and exit
        check_results(args, mean_dist)

        open(status_dir + "step_12.txt", 'w').close()

    print("All steps complete")


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
