# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks DeepSSM
====================================================================
"""
import os
import shapeworks as sw
import platform
import DataAugmentationUtils
import DeepSSMUtils
import json
import platform


def Run_Pipeline(args):
    if args.tiny_test:
        print("\nRunning a tiny test.")

    print("\nStep 1. Get Data")
    '''
    Get femur data
    Femur data is downloaded in a zip folder to Data/
    It gets extracted to Output/deep_ssm
    '''
    dataset_name = "femur-v0"
    out_dir = "Output/deep_ssm/"
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    if platform.system() == "Darwin":
        # On MacOS, CPU PyTorch is hanging with parallel
        os.environ['OMP_NUM_THREADS'] = "1"

    if args.tiny_test:
        sw.data.download_subset(args.use_case, dataset_name, out_dir)
        partition = 4
    else:
        sw.data.download_and_unzip_dataset(dataset_name, out_dir)
        partition = 40

    input_dir = out_dir + dataset_name + '/'
    # Get image path list
    img_dir = input_dir + "groomed/images/"
    img_list = []
    for file in os.listdir(img_dir):
        img_list.append(img_dir + file)
    img_list = sorted(img_list)
    # Get particles path list
    model_dir = input_dir + "shape_models/femur/1024/"
    world_particle_list = []
    local_particle_list = []
    for file in os.listdir(model_dir):
        if "local" in file:
            local_particle_list.append(model_dir + file)
        if "world" in file:
            world_particle_list.append(model_dir + file)
    local_particle_list = sorted(local_particle_list)
    world_particle_list = sorted(world_particle_list)
    # split into train and test
    train_img_list = img_list[:partition]
    train_local_particle_list = local_particle_list[:partition]
    train_world_particle_list = world_particle_list[:partition]
    test_img_list = img_list[partition:]
    if args.tiny_test:
        test_img_list = test_img_list[:3]

    print("\n\n\nStep 2. Augment data\n")
    '''
    - num_samples is how many samples to generate 
    - num_dim is the number of PCA scores to use
    - percent_dim what percent of variablity to retain (used if num_dim is 0)
    - aug_type is the augmentation method to use (1 is based on just particles wheras 2 is based on images and particles)
    - sample type is the distribution to use for sampling. Can be gaussian, mixture, or kde
    '''
    num_samples = 4960
    num_dim = 6
    percent_variability = 0.95
    sampler_type = "kde"
    if args.tiny_test:
        num_samples = 2
        num_dim = 0
        percent_variability = 0.99
    aug_dir = out_dir + "Augmentation/"
    embedded_dim = DataAugmentationUtils.runDataAugmentation(aug_dir, train_img_list, train_local_particle_list,
                                                             num_samples, num_dim, percent_variability, sampler_type,
                                                             mixture_num=0, processes=1,
                                                             world_point_list=train_world_particle_list)
    aug_data_csv = aug_dir + "TotalData.csv"

    if not args.tiny_test and not args.verify:
        DataAugmentationUtils.visualizeAugmentation(aug_data_csv, "violin")

    print("\n\n\nStep 3. Reformat Data for Pytorch\n")
    '''
    If down_sample is true, model will train on images half the original size
    If whiten is true, images and PCA scores will be whitened 
    Hyper-parameter batch_size is for training
        Higher batch size will help speed up training but uses more cuda memory, if you get a memory error try reducing the batch size
    '''

    down_factor = 0.75
    down_dir = out_dir + 'DownsampledImages/'
    batch_size = 8
    loader_dir = out_dir + 'TorchDataLoaders/'
    DeepSSMUtils.getTrainValLoaders(loader_dir, aug_data_csv, batch_size, down_factor, down_dir)
    DeepSSMUtils.getTestLoader(loader_dir, test_img_list, down_factor, down_dir)

    print("\n\n\nStep 4. Train model.\n")
    # Define model parameters
    model_name = "femur_deepssm"
    model_parameters = {
        "model_name": model_name,
        "num_latent_dim": int(embedded_dim),
        "paths": {
            "out_dir": out_dir,
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
    config_file = out_dir + model_name + ".json"
    with open(config_file, "w") as outfile:
        json.dump(model_parameters, outfile, indent=2)
    # Train
    DeepSSMUtils.trainDeepSSM(config_file)

    print("\n\n\nStep 5. Predict with DeepSSM\n")
    '''
    Test DeepSSM
    '''
    PCA_scores_path = out_dir + "Augmentation/PCA_Particle_Info/"
    prediction_dir = out_dir + model_name + '/predictions/'
    DeepSSMUtils.testDeepSSM(config_file)
    print('Predicted particles saved at: ' + prediction_dir)

    # If tiny test or verify, check results and exit
    if args.tiny_test or args.verify:
        if args.tiny_test:
            if not os.path.exists("Output/deep_ssm/femur_deepssm/predictions/PCA_Predictions/predicted_pca_m07_L.particles"):
                print("tiny test failed")
                exit(-1)
            # TODO: verify full run
        print("Done with test, verification succeeded.")
        exit()

    print("\n\n\nStep 6. Analyze results.\n")
    '''
    Analyze DeepSSM
    '''
    DT_dir = input_dir + "groomed/distance_transforms/"
    out_dir = out_dir + "Results/"
    mean_prefix = input_dir + "shape_models/femur/mean/femur"
    avg_distance = DeepSSMUtils.analyzeResults(out_dir, DT_dir, prediction_dir + 'FT_Predictions/', mean_prefix)
    print("Average surface-to-surface distance from the original to predicted shape = " + str(avg_distance))
