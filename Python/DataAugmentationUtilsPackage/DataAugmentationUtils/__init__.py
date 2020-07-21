from DataAugmentationUtils import DataAugmentation

'''
Runs data augmentation and takes the following arguements:
- out_dir = directory to save augmented data and related info
- img_list = list of paths to images of real examples augmentation is based off of
- point_list = list of paths to .particle files associated with examples in img_list
- num_samples = how many samples to generate
- num_dim = number of dimensions to reduce data too when embedding
- aug_type = type of augmentaion to run, 1 is for aug based only on particles, 2 bases aug on images and particles
- sampler_type = type of distribution to represent embedded data with for sampling
'''
def RunDataAugmentation(out_dir, img_list, point_list, num_samples, num_dim=0, aug_type=1, sampler_type="KDE"):
    if aug_type == 1:
        print("Running point based data augmentation.")
        DataAugmentation.point_based_aug(out_dir, img_list, point_list, num_samples, num_dim, sampler_type)
        print("Done.")
    elif aug_type == 2:
        print("Running point and image based augmentation.")
        DataAugmentation.point_and_image_based_aug(out_dir, img_list, point_list, num_samples, num_dim, sampler_type)
        print("Done.")
    else:
        print("Error: aug_type unrecognized.\nSet to 1 for point based augmentation or 2 for image and point based augmentation.")