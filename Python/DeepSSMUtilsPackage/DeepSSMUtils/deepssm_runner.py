# DeepSSM Runner utility

import argparse
import os
import sys
import DeepSSMUtils
import shapeworks as sw
from shapeworks.utils import sw_message, sw_progress, sw_check_abort

import DeepSSMUtils.config_file as config


def run_training(project):
    sw_message("Running training...")
    # check if loaders already exist
    deepssm_dir = DeepSSMUtils.get_deepssm_dir(project)
    loader_dir = deepssm_dir + "/torch_loaders/"

    params = project.get_parameters("deepssm")
    batch_size = int(params.get("train_batch_size"))
    print(f"Batch size: {batch_size}")

    # create loader if not exists
    if not os.path.exists(loader_dir + "train"):
        sw_message("Creating loaders...")
        # create loaders
        DeepSSMUtils.prepare_data_loaders(project, batch_size, "train")
        DeepSSMUtils.prepare_data_loaders(project, batch_size, "val")
        pass

    # prepare config file
    config.prepare_project_config_file(project)
    config_file = "deepssm/configuration.json"

    # run training
    DeepSSMUtils.trainDeepSSM(project, config_file)


def run_deepssm(project_filename, command):
    # check if project file exists
    if not os.path.exists(project_filename):
        print("Project file not found.")
        sys.exit(1)
    project = sw.Project()
    project.load(project_filename)
    if command == "prep":
        pass
    elif command == "augment":
        pass
    elif command == "train":
        run_training(project)
    elif command == "test":
        pass
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
