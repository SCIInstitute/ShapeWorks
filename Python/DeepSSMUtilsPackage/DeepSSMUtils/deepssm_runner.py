# DeepSSM Runner utility

import argparse
import os
import sys
import DeepSSMUtils
import shapeworks as sw
from shapeworks.utils import sw_message, sw_progress, sw_check_abort

import DeepSSMUtils.config_file as config

sw.setup_console_logging()


def run_prep(project):
    """Prepare data for DeepSSM"""
    params = sw.DeepSSMParameters(project)

    # Create split
    val_split = params.get_validation_split()
    test_split = params.get_testing_split()
    train_split = 100.0 - val_split - test_split
    sw_message(f"Splitting data into {train_split}% training, {val_split}% validation, and {test_split}% testing")
    DeepSSMUtils.create_split(project, train_split, val_split, test_split)

    if params.get_model_mode() == "Existing Model":
        sw_message("Using existing model")
    else:

        sw_message("Shape Model: Training data only")
        groom_params = project.get_parameters("groom")
        groom_params.set("alignment_method", "Iterative Closest Point")
        groom_params.set("alignment_enabled", "True")
        groom_params.set("alignment_reference", "-1")
        project.set_parameters("groom", groom_params)

        # Step 2 - Groom training shapes
        print("Grooming training shapes...")
        DeepSSMUtils.groom_training_shapes(project)
        # re-read parameters after python
        groom_params = project.get_parameters("groom")
        # store for future steps
        groom_params.set("alignment_reference", groom_params.get("alignment_reference_chosen"))
        project.set_parameters("groom", groom_params)

        # Step 3 - Optimize training particles
        print("Optimize training particles...")
        DeepSSMUtils.optimize_training_particles(project)

        # Step 4. Optimize Validation Particles with Fixed Domains
        DeepSSMUtils.prep_project_for_val_particles(project)
        print("Grooming validation shapes...")
        DeepSSMUtils.groom_validation_shapes(project)
        print("Optimize validation particles...")
        optimize = sw.Optimize()
        optimize.SetUpOptimize(project)
        optimize.Run()

    # Step 5. Groom Training Images
    print("Grooming training images...")
    DeepSSMUtils.groom_training_images(project, DeepSSMUtils.get_split_indices(project, "train"))

    # Step 6. Groom Validation Images
    print("Grooming validation images...")
    DeepSSMUtils.groom_val_test_images(project, DeepSSMUtils.get_split_indices(project, "val"))

    params.set_prep_step_complete(True)
    params.set_prep_stage(5)
    params.save_to_project()
    project.save()


def run_augmentation(project):
    params = sw.DeepSSMParameters(project)

    sampler_type = params.get_aug_sampler_type().lower()

    print("Running data augmentation...")
    print(f"Sampler type: {sampler_type}")
    print(f"Augmentation number of samples: {params.get_aug_num_samples()}")
    print(f"Augmentation percent variability: {params.get_aug_percent_variability()}")

    num_dims = DeepSSMUtils.run_data_augmentation(project, params.get_aug_num_samples(),
                                                  0,  # num dims, set to zero to allow percent variability to be used
                                                  params.get_aug_percent_variability(), sampler_type, 0,  # mixture_num
                                                  4  # processes
                                                  )

    params.set_training_num_dims(num_dims)

    params.set_aug_step_complete(True)
    params.save_to_project()
    project.save()


def run_training(project):

    params = sw.DeepSSMParameters(project)
    # set to full model
    params.set_model_mode("Existing Model")
    # set augmentation type to KDE
    params.set_aug_sampler_type("KDE")
    # set augmentation samples to 500
    params.set_aug_num_samples(500)
    params.set_training_fine_tuning(False)
    params.save_to_project()
    project.save()

    sw_message("Running training...")
    # check if loaders already exist
    deepssm_dir = DeepSSMUtils.get_deepssm_dir(project)
    loader_dir = deepssm_dir + "/torch_loaders/"

    batch_size = params.get_training_batch_size()
    print(f"Batch size: {batch_size}")

    # create loader if not exists
### TMP    if not os.path.exists(loader_dir + "train") or not os.path.exists(loader_dir + "validation"):
    sw_message("Creating loaders...")
    # create loaders
    DeepSSMUtils.prepare_data_loaders(project, batch_size, "train")
    DeepSSMUtils.prepare_data_loaders(project, batch_size, "val")

    # prepare config file
    config.prepare_project_config_file(project)
    config_file = "deepssm/configuration.json"

    # run training
    DeepSSMUtils.trainDeepSSM(project, config_file)


def run_testing(project):
    """ Run DeepSSM testing """
    print("Running testing...")
    params = sw.DeepSSMParameters(project)
    batch_size = params.get_training_batch_size()

    test_indices = DeepSSMUtils.get_split_indices(project, "test")

    # DeepSSMUtils.groom_val_test_images(project, test_indices)

    DeepSSMUtils.prepare_data_loaders(project, batch_size, "test");

    config_file = f"deepssm/configuration.json"
    DeepSSMUtils.testDeepSSM(config_file)
    DeepSSMUtils.process_test_predictions(project, "deepssm/configuration.json")

    # save the project
    project.save()


def run_deepssm(project_filename, command):
    # check if project file exists
    if not os.path.exists(project_filename):
        print("Project file not found.")
        sys.exit(1)
    project = sw.Project()
    project.load(project_filename)
    if command == "prep":
        run_prep(project)
    elif command == "augment":
        run_augmentation(project)
    elif command == "train":
        run_training(project)
    elif command == "test":
        run_testing(project)
    else:
        print("Command not recognized. Please use one of the following commands: prep, augment, train, test")
        sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='DeepSSM Runner')
    # single project (swproj or xlsx) file
    parser.add_argument('project_filename', help='Project filename')
    # command (prep, augment, train, test)
    parser.add_argument('command', help='Command')

    args = parser.parse_args()
    run_deepssm(args.project_filename, args.command)
