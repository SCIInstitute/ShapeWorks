# Package for Conditional DeepSSM model support
import os

import shapeworks as sw
import argparse
from shapeworks.utils import sw_message, sw_progress, sw_check_abort
import DeepSSMUtils
from DeepSSMUtils import loaders
import torch
from torch.utils.data import DataLoader



def generate_data_loader(project_filenames: list[str]):
    sw_message("Generating Data Loader")
    sw_message("Project filenames: {}".format(project_filenames))
    datasets = []
    # PWD + conditional_deepssm
    root_dir = os.getcwd()
    conditional_deepssm_dir = os.getcwd() + "/conditional_deepssm/"
    loader_dir = conditional_deepssm_dir + "/loader/"
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

    sw_message("Concatenating datasets")
    # Start with the first dataset as the base
    base_dataset = datasets[0]
    # Iterate through the remaining datasets and concatenate them
    for dataset in datasets[1:]:
        base_dataset.concatenate(dataset)

    sw_message("Creating dataloader")
    batch_size = 8

    train_loader = DataLoader(
        dataset,
        batch_size=batch_size,
        shuffle=True,
        num_workers=8,
        pin_memory=torch.cuda.is_available()
    )
    train_path = loader_dir + 'train'
    torch.save(train_loader, train_path)
    sw_message(f"Training loader completed and saved to {train_path}")



def run_conditional_deepssm_training(project_filenames: list[str]):
    sw_message("Running Conditional DeepSSM Training")

    os.makedirs('status', exist_ok=True)
    if not os.path.exists('status/generate_data_loader'):
        generate_data_loader(project_filenames)
        open('status/generate_data_loader', 'a').close()




if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run Conditional DeepSSM Training')
    parser.add_argument('project_filenames', nargs='+', help='Project filenames')
    args = parser.parse_args()
    run_conditional_deepssm_training(args.project_filenames)
