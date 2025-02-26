# Jadie Adams
import os
import numpy as np
import csv
import random
import subprocess
import torch
from torch import nn
from torch.utils.data import DataLoader
import shapeworks as sw
from shapeworks.utils import sw_message
from torch.nn.functional import pad

random.seed(1)


######################## Data loading functions ####################################

def make_dir(dirPath):
    """ Makes a directory if it does not exist """
    if not os.path.exists(dirPath):
        os.makedirs(dirPath)


def get_train_val_loaders(loader_dir, data_csv, batch_size=1, down_factor=1, down_dir=None, train_split=0.80):
    """ Reads csv and makes both train and validation data loaders from it """
    sw_message("Creating training and validation torch loaders:")
    make_dir(loader_dir)
    images, scores, models, prefixes, centers = get_all_train_data(loader_dir, data_csv, down_factor, down_dir)
    images, scores, models, prefixes, centers = shuffle_data(images, scores, models, prefixes, centers)
    # split into train and validation (e.g. 80% vs 20%)
    cut = int(len(images) * train_split)
    sw_message("Turning to tensors...")
    train_data = DeepSSMdataset(images[:cut], scores[:cut], models[:cut], prefixes[:cut], centers[:cut])
    sw_message(str(len(train_data)) + ' in training set')
    val_data = DeepSSMdataset(images[cut:], scores[cut:], models[cut:], prefixes[cut:], centers[:cut])
    sw_message(str(len(val_data)) + ' in validation set')

    sw_message("Saving data loaders...")
    trainloader = DataLoader(
        train_data,
        batch_size=batch_size,
        shuffle=True,
        num_workers=8,
        pin_memory=torch.cuda.is_available()
    )
    train_path = loader_dir + 'train'
    torch.save(trainloader, train_path)

    validationloader = DataLoader(
        val_data,
        batch_size=1,
        shuffle=True,
        num_workers=8,
        pin_memory=torch.cuda.is_available()
    )
    val_path = loader_dir + 'validation'
    torch.save(validationloader, val_path)
    sw_message("Training and validation loaders complete.\n")
    return train_path, val_path


def get_train_dataset(loader_dir, data_csv, down_factor=1, down_dir=None, anatomy=1):
    sw_message("Creating training dataset...")
    make_dir(loader_dir)
    images, scores, models, prefixes, centers = get_all_train_data(loader_dir, data_csv, down_factor, down_dir)
    images, scores, models, prefixes, centers = shuffle_data(images, scores, models, prefixes, centers)
    train_data = DeepSSMdataset(images, scores, models, prefixes, centers, anatomy)
    return train_data


def get_train_loader(loader_dir, data_csv, batch_size=1, down_factor=1, down_dir=None):
    """ Reads csv and creates a training data loader """
    sw_message("Creating training torch loader...")
    train_data = get_train_dataset(loader_dir, data_csv, down_factor, down_dir)

    sw_message(f"Training data dimensions: {len(train_data)}")
    train_loader = DataLoader(
        train_data,
        batch_size=batch_size,
        shuffle=True,
        num_workers=8,
        pin_memory=torch.cuda.is_available()
    )

    # print shape
    for img, _, _, _, _ in train_loader:
        print(f"Train image dimensions: {img.shape}")
        break

    train_path = loader_dir + 'train'
    torch.save(train_loader, train_path)
    sw_message("Training loader complete.")
    return train_path


def get_validation_dataset(loader_dir, val_img_list, val_particles, down_factor=1, down_dir=None, anatomy=1):
    """ Creates a validation dataset """
    sw_message("Creating validation dataset:")
    # Get data
    image_paths = []
    scores = []
    models = []
    names = []
    for index in range(len(val_img_list)):
        image_path = val_img_list[index]
        # add name
        prefix = get_prefix(image_path)
        names.append(prefix)
        image_paths.append(image_path)
        scores.append([1])  # placeholder
        mdl = get_particles(val_particles[index])
        models.append(mdl)
    # Write test names to file so they are saved somewhere
    name_file = open(loader_dir + 'validation_names.txt', 'w+')
    name_file.write(str(names))
    name_file.close()
    sw_message("Validation names saved to: " + loader_dir + "validation_names.txt")
    images, centers = get_images(loader_dir, image_paths, down_factor, down_dir)
    val_data = DeepSSMdataset(images, scores, models, names, centers, anatomy)
    sw_message("Validation dataset complete.")
    return val_data


def get_validation_loader(loader_dir, val_img_list, val_particles, down_factor=1, down_dir=None):
    """ Creates a validation data loader """
    sw_message("Creating validation torch loader:")
    val_data = get_validation_dataset(loader_dir, val_img_list, val_particles, down_factor, down_dir)
    # Make loader
    val_loader = DataLoader(
        val_data,
        batch_size=1,
        shuffle=False,
        num_workers=8,
        pin_memory=torch.cuda.is_available()
    )
    val_path = loader_dir + 'validation'
    torch.save(val_loader, val_path)
    sw_message("Validation loader complete.")
    return val_path


def get_test_loader(loader_dir, test_img_list, down_factor=1, down_dir=None):
    """ Creates a test data loader """
    sw_message("Creating test torch loader...")
    # get data
    image_paths = []
    scores = []
    models = []
    test_names = []
    for index in range(len(test_img_list)):
        image_path = test_img_list[index]
        # add name
        prefix = get_prefix(image_path)
        test_names.append(prefix)
        image_paths.append(image_path)
        # add label placeholders
        scores.append([1])
        models.append([1])
    images, centers = get_images(loader_dir, image_paths, down_factor, down_dir)
    test_data = DeepSSMdataset(images, scores, models, test_names, centers)
    # Write test names to file so they are saved somewhere
    name_file = open(loader_dir + 'test_names.txt', 'w+')
    name_file.write(str(test_names))
    name_file.close()
    sw_message("Test names saved to: " + loader_dir + "test_names.txt")
    # Make loader
    testloader = DataLoader(
        test_data,
        batch_size=1,
        shuffle=False,
        num_workers=8,
        pin_memory=torch.cuda.is_available()
    )
    test_path = loader_dir + 'test'
    torch.save(testloader, test_path)
    sw_message("Test loader complete.")
    return test_path, test_names


################################ Helper functions ######################################

def get_all_train_data(loader_dir, data_csv, down_factor, down_dir):
    """ returns images, scores, models, prefixes from CSV """
    # get all data and targets
    image_paths = []
    scores = []
    models = []
    prefixes = []
    with open(data_csv, newline='') as csvfile:
        datareader = csv.reader(csvfile)
        index = 0
        for row in datareader:
            image_path = row[0]
            model_path = row[1]
            pca_scores = row[2:]
            # add name
            prefix = get_prefix(image_path)
            prefixes.append(prefix)
            # add image path
            image_paths.append(image_path)
            # add score (un-normalized)
            pca_scores = [float(i) for i in pca_scores]
            scores.append(pca_scores)
            # add model
            mdl = get_particles(model_path)
            models.append(mdl)
            index += 1

    # # limit to 10
    # if len(image_paths) > 10:
    #     image_paths = image_paths[:10]
    #     scores = scores[:10]
    #     models = models[:10]
    #     prefixes = prefixes[:10]

    images, centers = get_images(loader_dir, image_paths, down_factor, down_dir)
    scores = whiten_PCA_scores(scores, loader_dir)
    return images, scores, models, prefixes, centers


def shuffle_data(images, scores, models, prefixes, centers):
    """ Shuffles all data """
    c = list(zip(images, scores, models, prefixes, centers))
    random.shuffle(c)
    images, scores, models, prefixes, centers = zip(*c)
    return images, scores, models, prefixes, centers


'''
Class for DeepSSM datasets that works with Pytorch DataLoader
'''


class DeepSSMdataset():
    def __init__(self, img, pca_target, mdl_target, names, centers, anatomy=1):
        self.img = torch.FloatTensor(np.array(img))
        self.pca_target = torch.FloatTensor(np.array(pca_target))
        self.mdl_target = torch.FloatTensor(np.array(mdl_target))
        self.names = names
        self.centers = centers
        self.anatomies = [anatomy] * len(img)
        # check that centers is a list of numpy.ndarray
        if not isinstance(centers[0], np.ndarray):
            print(f"Centers: {centers}")
            raise ValueError(f"Centers must be a list of numpy.ndarray, instead it seems to be a list of {type(centers[0])}")

    def __getitem__(self, index):
        x = self.img[index]
        y1 = self.pca_target[index]
        y2 = self.mdl_target[index]
        center = self.centers[index]
        name = self.names[index]
        if hasattr(self, 'anatomies'):
            anatomy = self.anatomies[index]
        else:
            anatomy = 0
        return x, y1, y2, name, anatomy, center

    def __len__(self):
        return len(self.img)

    def pad_images(self, current_shape, target_shape):
        # Calculate required padding for each dimension
        padding_list = []

        for current, target in zip(current_shape, target_shape):
            total_padding = target - current
            # Padding equally on both sides
            padding_list.extend([total_padding // 2, total_padding - total_padding // 2])

        print(f"Padding list: {padding_list}")

        # Pad all images in respective dimensions
        # `pad` expects padding in reverse order
        self.img = pad(self.img, padding_list[::-1], mode='constant', value=0)

        # Need to update centers as well
        # If the padding is even, the center should be the same
        # If the padding is odd, the center should be shifted by 1
        for i in range(len(self.centers)):
            center = self.centers[i]
            for j in range(len(center)):
                if target_shape[j] > current_shape[j]:
                    if padding_list[j] % 2 == 0:
                        continue
                    else:
                        center[j] += 1


    def concatenate(self, other):
        """ Concatenates two datasets """

        # Determine the largest shape
        max_shape = [
            max(self.img.shape[i], other.img.shape[i])
            for i in range(1, len(self.img.shape))
        ]

        print(f"Current shape: {self.img.shape[1:]}")
        print(f"Max shape: {max_shape}")

        # Pad both datasets to the max shape
        self.pad_images(self.img.shape[1:], max_shape)
        other.pad_images(other.img.shape[1:], max_shape)

        # Concatenate padded images and targets
        self.img = torch.cat((self.img, other.img), 0)
        # pad to the largest of the two
        if self.pca_target.shape[1] < other.pca_target.shape[1]:
            self.pca_target = pad(
                self.pca_target, (0, other.pca_target.shape[1] - self.pca_target.shape[1]), mode='constant', value=0)
        else:
            other.pca_target = pad(
                other.pca_target, (0, self.pca_target.shape[1] - other.pca_target.shape[1]), mode='constant', value=0)
        self.pca_target = torch.cat((self.pca_target, other.pca_target), 0)
        self.mdl_target = torch.cat((self.mdl_target, other.mdl_target), 0)
        self.names = self.names + other.names
        self.anatomies = self.anatomies + other.anatomies


def get_prefix(path):
    """ Returns sample prefix from path string """
    file_name = os.path.basename(path)
    prefix = file_name.split(".")[0]
    return prefix


def get_particles(model_path):
    """ Get list from .particles format """
    f = open(model_path, "r")
    data = []
    for line in f.readlines():
        points = line.split()
        points = [float(i) for i in points]
        data.append(points)
    return data


def get_images(loader_dir, image_list, down_factor, down_dir):
    """ reads .nrrd files and returns whitened data """
    # get all images
    all_images = []
    centers = []
    for image_path in image_list:
        if down_dir is not None:
            make_dir(down_dir)
            img_name = os.path.basename(image_path)
            res_img = os.path.join(down_dir, img_name)
            if not os.path.exists(res_img):
                apply_down_sample(image_path, res_img, down_factor)
            image_path = res_img
        # for_viewing returns 'F' order, i.e., transpose, needed for this array
        sw_img = sw.Image(image_path)
        img = sw_img.toArray(copy=True, for_viewing=True)
        all_images.append(img)
        print(f"Loaded image: {image_path}, center: {sw_img.center()}")
        centers.append(sw_img.center())

    all_images = np.array(all_images)

    # TODO: Problem: You can't do this differently for each group (test, train, val)

    # get mean and std
    mean_path = loader_dir + 'mean_img.npy'
    std_path = loader_dir + 'std_img.npy'
    mean_image = np.mean(all_images)
    std_image = np.std(all_images)
    np.save(mean_path, mean_image)
    np.save(std_path, std_image)
    # normalize
    norm_images = []
    for image in all_images:
        norm_images.append([(image - mean_image) / std_image])
    return norm_images, centers


def apply_down_sample(image_path, output_path, factor=0.75):
    """ decreases the size of the image """
    image = sw.Image(image_path)
    size = image.size()
    sizex = int(size[0] * factor)
    sizey = int(size[1] * factor)
    sizez = int(size[2] * factor)
    image.resize([sizex, sizey, sizez]).write(output_path)


def make_vector(str):
    """ turns a string list into a vector """
    arr = np.array(str.replace("[", "").replace("]", "").split(","))
    return np.asarray(arr, np.float64)


def whiten_PCA_scores(scores, loader_dir):
    """ normalizes PCA scores, returns mean and std for reconstruction """
    scores = np.array(scores)
    mean_score = np.mean(scores, 0)
    std_score = np.std(scores, 0)
    np.save(loader_dir + 'mean_PCA.npy', mean_score)
    np.save(loader_dir + 'std_PCA.npy', std_score)
    norm_scores = []
    for score in scores:
        norm_scores.append((score - mean_score) / std_score)
    return norm_scores
