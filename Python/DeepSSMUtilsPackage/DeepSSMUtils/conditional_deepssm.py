# Package for Conditional DeepSSM model support
import os
import argparse

import shapeworks as sw
from shapeworks.utils import sw_message, sw_progress, sw_check_abort

import torch
from torch.utils.data import DataLoader

# DeepSSM imports
import loaders
import trainer
import run_utils


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
    sw_message("Step: Generating Combined Data Loader")
    sw_message("Project filenames: {}".format(project_filenames))
    datasets = []
    val_datasets = []
    # PWD + conditional_deepssm
    root_dir = os.getcwd()
    conditional_deepssm_dir = os.getcwd() + "/deepssm/"
    loader_dir = conditional_deepssm_dir + "/torch_loaders/"
    for project_filename in project_filenames:
        os.chdir(root_dir)
        sw_message("Project filename: {}".format(project_filename))
        project_dir = os.path.abspath(os.path.dirname(project_filename))
        os.chdir(project_dir)
        sw_message("Project directory: {}".format(project_dir))
        sw_message(f"Current working directory: {os.getcwd()}")
        project_deepssm_dir = project_dir + "/deepssm/"

        project_aug_dir = project_deepssm_dir + "augmentation/"
        project_aug_data_csv = project_aug_dir + "TotalData.csv"

        dataset = loaders.get_train_dataset(loader_dir, project_aug_data_csv, down_factor=1, down_dir=None)
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
                                                     down_dir=None)
        val_datasets.append(val_dataset)

    sw_message("Creating dataloaders")
    batch_size = 8

    train_loader = DataLoader(
        concatenate_datasets(datasets),
        batch_size=batch_size,
        shuffle=True,
        num_workers=8,
        pin_memory=torch.cuda.is_available()
    )
    train_path = loader_dir + 'train'
    torch.save(train_loader, train_path)
    sw_message(f"Training loader completed and saved to {train_path}")

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


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run Conditional DeepSSM Training')
    parser.add_argument('project_filenames', nargs='+', help='Project filenames')
    args = parser.parse_args()
    run_conditional_deepssm_training(args.project_filenames)
