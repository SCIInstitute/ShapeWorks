# Package for Conditional DeepSSM model support
import os
import argparse
from fileinput import filename

import matplotlib.pyplot as plt
import PIL
import numpy as np

import DeepSSMUtils
import DataAugmentationUtils

import shapeworks as sw
from shapeworks.utils import sw_message, sw_progress, sw_check_abort

import torch
from torch.utils.data import DataLoader
import torch.nn.functional as F  # Useful for pad operations

# DeepSSM imports
import loaders
import trainer
import run_utils

import shutil


def get_image_dimensions(dataloader_file: str):
    """ Get the image dimensions from the dataloader file """
    dataloader = torch.load(dataloader_file)
    for img, pca, mdl, names, anatomy in dataloader:
        image_dimensions = img.shape[2:]
        return image_dimensions
    return None


def update_dataloader_with_padding2(dataloader_file: str, image_dimensions: tuple, anatomy: int):
    """ Update the dataloader by padding the images to match image_dimensions and update anatomy info """
    # Load the existing dataloader
    dataloader = torch.load(dataloader_file)
    updated_dataloader = []

    count = 0
    for img, pca, mdl, names, old_anatomy in dataloader:
        # Calculate padding sizes
        padding_needed = [(target - actual) if target > actual else 0 for target, actual in
                          zip(image_dimensions, img.shape[2:])]
        pad = (0, padding_needed[2], 0, padding_needed[1], 0, padding_needed[0])  # Assuming 3D (D, W, H)

        # Pad the image
        padded_img = F.pad(img, pad, "constant", 0)

        # Append the updated dataloader items
        updated_dataloader.append((padded_img, pca, mdl, names, anatomy))

        import nrrd
        numpy_array = padded_img.numpy()
        numpy_array = np.squeeze(numpy_array)

        print(f"Image dimensions: {numpy_array.shape}")

        # Write to NRRD file
        output_filename = f"/tmp/padded_{anatomy}_{count}.nrrd"
        nrrd.write(output_filename, numpy_array)

        count += 1

    # Save the updated dataloader
    torch.save(updated_dataloader, dataloader_file)


def update_dataloader_with_padding(dataloader_file: str, image_dimensions: tuple, anatomy: int):
    """ Update the dataloader by padding the images to match image_dimensions and update anatomy info """
    # Load the existing dataloader
    dataloader = torch.load(dataloader_file)
    updated_dataloader = []

    count = 0
    for img, pca, mdl, names, old_anatomy in dataloader:
        # Calculate padding sizes for each dimension
        padding_needed = [(target - actual) if target > actual else 0 for target, actual in
                          zip(image_dimensions, img.shape[2:])]

        # Calculate padding values for both sides to center the image
        pad_d = (padding_needed[0] // 2, padding_needed[0] - padding_needed[0] // 2)
        pad_h = (padding_needed[1] // 2, padding_needed[1] - padding_needed[1] // 2)
        pad_w = (padding_needed[2] // 2, padding_needed[2] - padding_needed[2] // 2)

        pad = (pad_w[0], pad_w[1], pad_h[0], pad_h[1], pad_d[0], pad_d[1])  # Assuming 3D (D, H, W)

        # Pad the image
        padded_img = F.pad(img, pad, "constant", 0)

        # Append the updated dataloader items
        updated_dataloader.append((padded_img, pca, mdl, names, anatomy))

        do_write = False
        if do_write:
            # Convert the tensor to a numpy array and squeeze it if necessary
            numpy_array = padded_img.numpy()
            numpy_array = np.squeeze(numpy_array)

            print(f"Image dimensions after padding: {numpy_array.shape}")

            import nrrd
            # Write to NRRD file
            output_filename = f"/tmp/padded_{anatomy}_{count}.nrrd"
            nrrd.write(output_filename, numpy_array)

            count += 1

    # Save the updated dataloader
    torch.save(updated_dataloader, dataloader_file)


def concatenate_datasets(datasets: list):
    """ Concatenate a list of datasets """
    sw_message("Concatenating datasets")
    # Start with the first dataset as the base
    base_dataset = datasets[0]
    # Iterate through the remaining datasets and concatenate them
    for dataset in datasets[1:]:
        base_dataset.concatenate(dataset)
    return base_dataset


def generate_data_loaders(project_filenames: list[str]):
    """ Generate a data loader for the Conditional DeepSSM model """
    print("")
    sw_message("Step: Generating Combined Data Loader")
    sw_message("Project filenames: {}".format(project_filenames))
    datasets = []
    val_datasets = []
    # PWD + conditional_deepssm
    root_dir = os.getcwd()
    conditional_deepssm_dir = os.getcwd() + "/deepssm/"
    loader_dir = conditional_deepssm_dir + "/torch_loaders/"
    for anatomy in range(len(project_filenames)):
        project_filename = project_filenames[anatomy]
        os.chdir(root_dir)
        print("")
        sw_message("Project filename: {}".format(project_filename))
        project_dir = os.path.abspath(os.path.dirname(project_filename))
        os.chdir(project_dir)
        project_deepssm_dir = project_dir + "/deepssm/"

        project_aug_dir = project_deepssm_dir + "augmentation/"
        project_aug_data_csv = project_aug_dir + "TotalData.csv"

        dataset = loaders.get_train_dataset(loader_dir, project_aug_data_csv, down_factor=1, down_dir=None,
                                            anatomy=anatomy)
        datasets.append(dataset)

        project = sw.Project()
        project.load(os.path.basename(project_filename))

        val_image_files = []
        val_world_particles = []
        val_indices = run_utils.get_split_indices(project, "val")
        for i in val_indices:
            val_image_files.append(project_deepssm_dir + "images/" + f"{i}.nrrd")
            particle_file = project.get_subjects()[i].get_world_particle_filenames()[0]
            val_world_particles.append(particle_file)

        val_dataset = loaders.get_validation_dataset(loader_dir, val_image_files, val_world_particles, down_factor=1,
                                                     down_dir=None, anatomy=anatomy)
        val_datasets.append(val_dataset)

    print("")
    sw_message("Creating dataloaders")
    batch_size = 8

    train_loader = DataLoader(
        concatenate_datasets(datasets),
        batch_size=batch_size,
        shuffle=True,
        num_workers=8,
        pin_memory=torch.cuda.is_available()
    )

    print("Number of images in training loader: ", len(train_loader))

    train_path = loader_dir + 'train'
    torch.save(train_loader, train_path)
    sw_message(f"Training loader completed and saved to {train_path}")

    print("Training image dimensions: ", get_image_dimensions(train_path))

    val_loader = DataLoader(
        concatenate_datasets(val_datasets),
        batch_size=1,
        shuffle=False,
        num_workers=8,
        pin_memory=torch.cuda.is_available()
    )
    val_path = loader_dir + 'validation'
    torch.save(val_loader, val_path)
    sw_message(f"Validation loader completed and saved to {val_path}")

    os.chdir(root_dir)


def run_conditional_deepssm_training(project_filenames: list[str]):
    """ Run Conditional DeepSSM Pipeline """
    sw_message("Running Conditional DeepSSM Training")

    os.makedirs('status', exist_ok=True)
    if not os.path.exists('status/generate_data_loader'):
        generate_data_loaders(project_filenames)
        open('status/generate_data_loader', 'a').close()

    # assume configuration.json already exists, for now
    config_file = "deepssm/configuration.json"

    trainer.train(None, config_file)


def run_conditional_deepssm_prep(project_filenames: list[str]):
    """ Prepare data for Conditional DeepSSM """
    sw_message("Preparing data for Conditional DeepSSM")

    root_dir = os.getcwd()

    # for each project
    for project_filename in project_filenames:
        os.chdir(root_dir)
        if not os.path.exists(project_filename):
            print(f"Project file {project_filename} does not exist")
            continue

        # chdir to project directory
        project_dir = os.path.abspath(os.path.dirname(project_filename))
        os.chdir(project_dir)
        print(f"Working in project directory: {project_dir}")
        project_filename = os.path.basename(project_filename)

        print(f"Preparing project: {project_filename}")
        project = sw.Project()
        project.load(os.path.basename(project_filename))
        params = sw.DeepSSMParameters(project)
        # set to full model
        params.set_model_mode("Existing Model")
        # set augmentation type to KDE
        params.set_aug_sampler_type("KDE")
        # set augmentation samples to 500
        params.set_aug_num_samples(500)
        # save the project
        project.save()

        # define split
        DeepSSMUtils.create_split(project, 70, 15, 15)

        train_indices = DeepSSMUtils.get_split_indices(project, "train")
        val_indices = DeepSSMUtils.get_split_indices(project, "val")
        test_indices = DeepSSMUtils.get_split_indices(project, "test")

        all_indices = train_indices + val_indices + test_indices
        print("Number of indices: ", len(all_indices))

        print("Grooming training images")
        DeepSSMUtils.groom_training_images(project, all_indices)

        # run data augmentation
        print("Running data augmentation")
        num_samples = 500
        num_dim = 0

        # percent_variability = 0.95
        percent_variability = 0.95
        sampler = "kde"
        #        sampler = "gaussian"
        embedded_dim = DeepSSMUtils.run_data_augmentation(project, num_samples, num_dim, percent_variability, sampler,
                                                          mixture_num=0, processes=4)


def run_conditional_deepssm_testing(project_filenames: list[str]):
    """ Run Conditional DeepSSM Testing """
    sw_message("Running Conditional DeepSSM Testing")

    root_dir = os.getcwd()

    for anatomy in range(len(project_filenames)):
        project_filename = project_filenames[anatomy]
        print(f"Project filename: {project_filename}")
        os.chdir(root_dir)
        if not os.path.exists(project_filename):
            print(f"Project file {project_filename} does not exist")
            continue

        # chdir to project directory
        project_dir = os.path.abspath(os.path.dirname(project_filename))
        os.chdir(project_dir)
        print(f"Working in project directory: {project_dir}")
        project_filename = os.path.basename(project_filename)

        print(f"Testing project: {project_filename}")
        project = sw.Project()
        project.load(os.path.basename(project_filename))

        params = sw.DeepSSMParameters(project)
        batch_size = params.get_training_batch_size()

        test_indices = DeepSSMUtils.get_split_indices(project, "test")

        print(f"Grooming test images for project: {project_filename}")
        DeepSSMUtils.groom_val_test_images(project, test_indices)

        print(f"Preparing data loaders for project: {project_filename}")
        DeepSSMUtils.prepare_data_loaders(project, batch_size, "test");

        # save the project
        project.save()

        # copy the test loader to the root directory
        shutil.copy(f"{project_dir}/deepssm/torch_loaders/test", f"{root_dir}/deepssm/torch_loaders/test")
        # copy the test_names.txt file
        shutil.copy(f"{project_dir}/deepssm/torch_loaders/test_names.txt",
                    f"{root_dir}/deepssm/torch_loaders/test_names.txt")

        os.chdir(root_dir)

        # get the image dimensions
        image_dimensions = get_image_dimensions("deepssm/torch_loaders/test")
        print("Test Image dimensions: ", image_dimensions)

        train_dimensions = get_image_dimensions("deepssm/torch_loaders/train")
        print("Train Image dimensions: ", train_dimensions)

        # now we have to update the test loader to match the image dimensions of the training images and set the anatomy
        update_dataloader_with_padding("deepssm/torch_loaders/test", train_dimensions, anatomy)
        print("Using anatomy: ", anatomy)

        config_file = f"deepssm/configuration.json"

        # clean out the deepssm/model/test_predictions/world_predictions directory
        test_pred_dir = f"deepssm/model/test_predictions/world_predictions"
        if os.path.exists(test_pred_dir):
            shutil.rmtree(test_pred_dir)

        DeepSSMUtils.testDeepSSM(config_file)

        # copy config file to the project directory
        shutil.copy(config_file, f"{project_dir}/deepssm/configuration.json")

        # copy the test_predictions directory to the project directory
        project_test_pred_dir = f"{project_dir}/deepssm/model/test_predictions"
        if os.path.exists(project_test_pred_dir):
            shutil.rmtree(project_test_pred_dir)
        shutil.copytree(test_pred_dir, f"{project_dir}/deepssm/model/test_predictions/world_predictions")

        # switch back to the project directory
        os.chdir(project_dir)
        # process the test predictions
        DeepSSMUtils.process_test_predictions(project, "deepssm/configuration.json")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run Conditional DeepSSM Training')

    ## --train, --test flags
    parser.add_argument('--train', action='store_true', help='Run training')
    parser.add_argument('--test', action='store_true', help='Run testing')
    parser.add_argument("--prep", action='store_true', help='Prepare data for training')

    parser.add_argument('project_filenames', nargs='+', help='Project filenames')

    args = parser.parse_args()

    if args.train:
        run_conditional_deepssm_training(args.project_filenames)
    elif args.test:
        run_conditional_deepssm_testing(args.project_filenames)
    elif args.prep:
        run_conditional_deepssm_prep(args.project_filenames)
    else:
        print("Please specify either --train or --test flag")
        exit(1)
