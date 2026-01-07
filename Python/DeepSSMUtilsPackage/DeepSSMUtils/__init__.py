from typing import List, Optional, Tuple, Any

from DeepSSMUtils import trainer
from DeepSSMUtils import loaders
from DeepSSMUtils import eval
from DeepSSMUtils import eval_utils
from DeepSSMUtils import config_file
from DeepSSMUtils import train_viz
from DeepSSMUtils import image_utils
from DeepSSMUtils import run_utils
from DeepSSMUtils import net_utils
from DeepSSMUtils import constants
from DeepSSMUtils import config_validation

from .net_utils import set_seed

from .run_utils import create_split, groom_training_shapes, groom_training_images, \
    run_data_augmentation, groom_val_test_images, prep_project_for_val_particles, groom_validation_shapes, \
    prepare_data_loaders, get_deepssm_dir, get_split_indices, optimize_training_particles, process_test_predictions

from .config_file import prepare_config_file

import torch


def getTrainValLoaders(
    loader_dir: str,
    aug_data_csv: str,
    batch_size: int = 1,
    down_factor: float = 1,
    down_dir: Optional[str] = None,
    train_split: float = 0.80,
    num_workers: int = 0
) -> None:
    """Create training and validation data loaders from augmented data CSV."""
    testPytorch()
    loaders.get_train_val_loaders(loader_dir, aug_data_csv, batch_size, down_factor, down_dir, train_split, num_workers)


def getTrainLoader(
    loader_dir: str,
    aug_data_csv: str,
    batch_size: int = 1,
    down_factor: float = 1,
    down_dir: Optional[str] = None,
    train_split: float = 0.80,
    num_workers: int = 0
) -> None:
    """Create training data loader from augmented data CSV."""
    testPytorch()
    loaders.get_train_loader(loader_dir, aug_data_csv, batch_size, down_factor, down_dir, train_split, num_workers)


def getValidationLoader(
    loader_dir: str,
    val_img_list: List[str],
    val_particles: List[str],
    down_factor: float = 1,
    down_dir: Optional[str] = None,
    num_workers: int = 0
) -> None:
    """Create validation data loader from image and particle lists."""
    loaders.get_validation_loader(loader_dir, val_img_list, val_particles, down_factor, down_dir, num_workers)


def getTestLoader(
    loader_dir: str,
    test_img_list: List[str],
    down_factor: float = 1,
    down_dir: Optional[str] = None,
    num_workers: int = 0
) -> None:
    """Create test data loader from image list."""
    loaders.get_test_loader(loader_dir, test_img_list, down_factor, down_dir, num_workers)


def prepareConfigFile(
    config_filename: str,
    model_name: str,
    embedded_dim: int,
    out_dir: str,
    loader_dir: str,
    aug_dir: str,
    epochs: int,
    learning_rate: float,
    decay_lr: bool,
    fine_tune: bool,
    fine_tune_epochs: int,
    fine_tune_learning_rate: float
) -> None:
    """Prepare a DeepSSM configuration file with the specified parameters."""
    config_file.prepare_config_file(config_filename, model_name, embedded_dim, out_dir, loader_dir, aug_dir, epochs,
                                    learning_rate, decay_lr, fine_tune,
                                    fine_tune_epochs, fine_tune_learning_rate)


def trainDeepSSM(project: Any, config_file: str) -> None:
    """Train a DeepSSM model using the given project and configuration file."""
    testPytorch()
    trainer.train(project, config_file)
    return


def testDeepSSM(config_file: str, loader: str = "test") -> List[str]:
    """
    Test a trained DeepSSM model and return predicted particle files.

    Args:
        config_file: Path to the configuration JSON file
        loader: Which loader to use ("test" or "validation")

    Returns:
        List of paths to predicted particle files
    """
    predicted_particle_files = eval.test(config_file, loader)
    return predicted_particle_files


def analyzeMSE(
    predicted_particles: List[str],
    true_particles: List[str]
) -> Tuple[float, float]:
    """
    Analyze mean squared error between predicted and true particles.

    Returns:
        Tuple of (mean_MSE, std_MSE)
    """
    mean_MSE, STD_MSE = eval_utils.get_MSE(predicted_particles, true_particles)
    return mean_MSE, STD_MSE


def analyzeMeshDistance(
    predicted_particles: List[str],
    mesh_files: List[str],
    template_particles: str,
    template_mesh: str,
    out_dir: str,
    planes: Optional[Any] = None
) -> float:
    """
    Analyze mesh distance between predicted particles and ground truth meshes.

    Returns:
        Mean surface-to-surface distance
    """
    mean_distance = eval_utils.get_mesh_distance(predicted_particles, mesh_files, template_particles,
                                                 template_mesh, out_dir, planes)
    return mean_distance


def analyzeResults(
    out_dir: str,
    DT_dir: str,
    prediction_dir: str,
    mean_prefix: str
) -> float:
    """
    Analyze results by computing distance between predicted and ground truth meshes.

    Returns:
        Average surface distance
    """
    avg_distance = eval_utils.get_distance_meshes(out_dir, DT_dir, prediction_dir, mean_prefix)
    return avg_distance


def get_image_registration_transform(
    fixed_image_file: str,
    moving_image_file: str,
    transform_type: str = 'rigid'
) -> Any:
    """
    Compute image registration transform between two images.

    Args:
        fixed_image_file: Path to the fixed/reference image
        moving_image_file: Path to the moving image to be registered
        transform_type: Type of transform ('rigid', 'affine', etc.)

    Returns:
        ITK transform object
    """
    itk_transform = image_utils.get_image_registration_transform(fixed_image_file, moving_image_file,
                                                                 transform_type=transform_type)
    return itk_transform


def testPytorch() -> None:
    """Check if PyTorch is using GPU and print a warning if not."""
    if torch.cuda.is_available():
        print("Running on GPU.")
    else:
        print("********************* WARNING ****************************")
        print("Pytorch is running on your CPU!")
        print("This will be very slow. If your machine has a GPU,")
        print("please reinstall Pytorch to your shapeworks conda ")
        print("environment with the correct CUDA version.")
        print("**********************************************************")
