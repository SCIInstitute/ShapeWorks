from DeepSSMUtils import trainer
from DeepSSMUtils import loaders
from DeepSSMUtils import eval
from DeepSSMUtils import eval_utils
from DeepSSMUtils import config_file
from DeepSSMUtils import train_viz
from DeepSSMUtils import image_utils
from DeepSSMUtils import run_utils

from .run_utils import create_split, groom_training_shapes, groom_training_images, \
    run_data_augmentation, groom_val_test_images, prep_project_for_val_particles, groom_validation_shapes, \
    prepare_data_loaders, get_deepssm_dir, get_split_indices, optimize_training_particles, process_test_predictions

from .config_file import prepare_config_file

import torch


def getTrainValLoaders(loader_dir, aug_data_csv, batch_size=1, down_factor=1, down_dir=None, train_split=0.80):
    testPytorch()
    loaders.get_train_val_loaders(loader_dir, aug_data_csv, batch_size, down_factor, down_dir, train_split)


def getTrainLoader(loader_dir, aug_data_csv, batch_size=1, down_factor=1, down_dir=None, train_split=0.80):
    testPytorch()
    loaders.get_train_loader(loader_dir, aug_data_csv, batch_size, down_factor, down_dir, train_split)


def getValidationLoader(loader_dir, val_img_list, val_particles, down_factor=1, down_dir=None):
    loaders.get_validation_loader(loader_dir, val_img_list, val_particles, down_factor, down_dir)


def getTestLoader(loader_dir, test_img_list, down_factor=1, down_dir=None):
    loaders.get_test_loader(loader_dir, test_img_list, down_factor, down_dir)


def prepareConfigFile(config_filename, model_name, embedded_dim, out_dir, loader_dir, aug_dir, epochs, learning_rate,
                      decay_lr, fine_tune, fine_tune_epochs, fine_tune_learning_rate):
    config_file.prepare_config_file(config_filename, model_name, embedded_dim, out_dir, loader_dir, aug_dir, epochs,
                                    learning_rate, decay_lr, fine_tune,
                                    fine_tune_epochs, fine_tune_learning_rate)


def trainDeepSSM(project, config_file):
    testPytorch()
    trainer.train(project, config_file)
    return


def testDeepSSM(config_file, loader="test"):
    predicted_particle_files = eval.test(config_file, loader)
    return predicted_particle_files


def analyzeMSE(predicted_particles, true_particles):
    mean_MSE, STD_MSE = eval_utils.get_MSE(predicted_particles, true_particles)
    return mean_MSE, STD_MSE


def analyzeMeshDistance(predicted_particles, mesh_files, template_particles, template_mesh, out_dir, planes=None):
    mean_distance = eval_utils.get_mesh_distance(predicted_particles, mesh_files, template_particles,
                                                 template_mesh, out_dir, planes)
    return mean_distance


def analyzeResults(out_dir, DT_dir, prediction_dir, mean_prefix):
    avg_distance = eval_utils.get_distance_meshes(out_dir, DT_dir, prediction_dir, mean_prefix)
    return avg_distance


def get_image_registration_transform(fixed_image_file, moving_image_file, transform_type='rigid'):
    itk_transform = image_utils.get_image_registration_transform(fixed_image_file, moving_image_file,
                                                                 transform_type=transform_type)
    return itk_transform


def testPytorch():
    if torch.cuda.is_available():
        print("Running on GPU.")
    else:
        print("********************* WARNING ****************************")
        print("Pytorch is running on your CPU!")
        print("This will be very slow. If your machine has a GPU,")
        print("please reinstall Pytorch to your shapeworks conda ")
        print("environment with the correct CUDA version.")
        print("**********************************************************")
