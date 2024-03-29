from DataAugmentationUtils import DataAugmentation
from DataAugmentationUtils import Visualize
from DataAugmentationUtils import Utils
from shapeworks.utils import sw_message

'''
Runs data augmentation and takes the following arguments:
- out_dir = directory to save augmented data and related info
- img_list = list of paths to images of real examples augmentation is based off of
- local_point_list = list of paths to local.particle files associated with examples in img_list
- num_samples = how many samples to generate
- num_dim = number of dimensions to reduce data too when embedding
- aug_type = type of augmentation to run, 1 is for aug based only on particles, 2 bases aug on images and particles
- sampler_type = type of distribution to represent embedded data with for sampling
- processes = number of processes to break image generation between for parallelization
- world_point_list = list of paths to local.particle files (required if different from local)
'''


def runDataAugmentation(out_dir, img_list, world_point_list, num_samples=3, num_dim=0, percent_variability=0.95, sampler_type="KDE", mixture_num=0, processes=1):
    sw_message("Running point based data augmentation.")
    num_dim = DataAugmentation.point_based_aug(out_dir, img_list, world_point_list, num_samples, num_dim, percent_variability, sampler_type, mixture_num, processes)
    sw_message("Done.")
    return num_dim

def runLocalDataAugmentation(out_dir, img_list, local_point_list, world_point_list, num_samples=3, num_dim=0, percent_variability=0.95, sampler_type="KDE", mixture_num=0, processes=1):
    sw_message("Running point based data augmentation.")
    num_dim = DataAugmentation.point_based_aug(out_dir, img_list, local_point_list, num_samples, num_dim, percent_variability, sampler_type, mixture_num, processes, world_point_list)
    sw_message("Done.")
    return num_dim


def visualizeAugmentation(data_csv, viz_type='splom', show=True):
    if viz_type == 'splom':
        Visualize.splom(data_csv)
    elif viz_type == 'violin':
        Visualize.violin(data_csv, show)
    else:
        sw_message("Error visualization type unrecognized.")

