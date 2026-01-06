"""
Constants used throughout DeepSSM.

This module centralizes magic strings and default values to improve
maintainability and reduce errors from typos.
"""

# Model file names
BEST_MODEL_FILE = "best_model.torch"
FINAL_MODEL_FILE = "final_model.torch"
BEST_MODEL_FT_FILE = "best_model_ft.torch"
FINAL_MODEL_FT_FILE = "final_model_ft.torch"
FINAL_MODEL_AE_FILE = "final_model_ae.torch"
FINAL_MODEL_TF_FILE = "final_model_tf.torch"

# Data loader names
TRAIN_LOADER = "train"
VALIDATION_LOADER = "validation"
TEST_LOADER = "test"

# File names for saved statistics
MEAN_PCA_FILE = "mean_PCA.npy"
STD_PCA_FILE = "std_PCA.npy"
MEAN_IMG_FILE = "mean_img.npy"
STD_IMG_FILE = "std_img.npy"

# Names files
TRAIN_NAMES_FILE = "train_names.txt"
VALIDATION_NAMES_FILE = "validation_names.txt"
TEST_NAMES_FILE = "test_names.txt"

# Log and plot files
TRAIN_LOG_FILE = "train_log.csv"
TRAINING_PLOT_FILE = "training_plot.png"
TRAINING_PLOT_FT_FILE = "training_plot_ft.png"
TRAINING_PLOT_AE_FILE = "training_plot_ae.png"
TRAINING_PLOT_TF_FILE = "training_plot_tf.png"
TRAINING_PLOT_JOINT_FILE = "training_plot_joint.png"

# PCA info directory and files
PCA_INFO_DIR = "PCA_Particle_Info"
PCA_MEAN_FILE = "mean.particles"
PCA_MODE_FILE_TEMPLATE = "pcamode{}.particles"

# Prediction directories
WORLD_PREDICTIONS_DIR = "world_predictions"
PCA_PREDICTIONS_DIR = "pca_predictions"
LOCAL_PREDICTIONS_DIR = "local_predictions"

# Examples directory
EXAMPLES_DIR = "examples"
TRAIN_EXAMPLES_PREFIX = "train_"
VALIDATION_EXAMPLES_PREFIX = "validation_"

# Training stage names (for logging)
class TrainingStage:
    BASE = "Base_Training"
    FINE_TUNING = "Fine_Tuning"
    AUTOENCODER = "AE"
    T_FLANK = "T-Flank"
    JOINT = "Joint"

# Default values
class Defaults:
    BATCH_SIZE = 1
    DOWN_FACTOR = 1
    TRAIN_SPLIT = 0.80
    NUM_WORKERS = 0
    VAL_FREQ = 1

# Device strings
DEVICE_CUDA = "cuda:0"
DEVICE_CPU = "cpu"
