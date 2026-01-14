# Jadie Adams
import os
import numpy as np
import csv
import random
import subprocess
import torch
from torch import nn
from torch.utils.data import DataLoader, Dataset
import shapeworks as sw
from shapeworks.utils import sw_message
from DeepSSMUtils import constants as C
random.seed(1)

# Use streaming data loading to avoid loading all images into memory
USE_STREAMING = True


class DataLoadingError(Exception):
    """Raised when data loading fails."""
    pass

######################## Data loading functions ####################################

'''
Make folder
'''
def make_dir(dirPath):
    if not os.path.exists(dirPath):
        os.makedirs(dirPath)


'''
Load a DataLoader from a saved file. Handles both streaming (metadata) and legacy (full loader) formats.
'''
def load_data_loader(loader_path, loader_type='train'):
	data = torch.load(loader_path, weights_only=False)

	# Check if it's streaming metadata or a full DataLoader
	if isinstance(data, dict) and data.get('streaming', False):
		# Reconstruct streaming DataLoader from metadata
		if loader_type == 'train':
			dataset = DeepSSMdatasetStreaming(
				data['image_paths'],
				data['scores'],
				data['models'],
				data['prefixes'],
				data['mean_img'],
				data['std_img']
			)
			return DataLoader(
				dataset,
				batch_size=data.get('batch_size', 1),
				shuffle=True,
				num_workers=data.get('num_workers', 0),
				pin_memory=torch.cuda.is_available()
			)
		else:
			# Validation or test
			dataset = DeepSSMdatasetStreaming(
				data['image_paths'],
				data['scores'],
				data['models'],
				data['names'],
				data['mean_img'],
				data['std_img']
			)
			return DataLoader(
				dataset,
				batch_size=1,
				shuffle=False,
				num_workers=data.get('num_workers', 0),
				pin_memory=torch.cuda.is_available()
			)
	else:
		# Legacy format - data is already a DataLoader
		return data


'''
Get dataset info (image dimensions, num_corr) from a loader file.
Works with both streaming and legacy formats.
'''
def get_loader_info(loader_path):
	data = torch.load(loader_path, weights_only=False)

	if isinstance(data, dict) and data.get('streaming', False):
		# Streaming format - load one image to get dimensions
		image_path = data['image_paths'][0]
		img = sw.Image(image_path).toArray(copy=True, for_viewing=True)
		img_dims = img.shape
		num_corr = len(data['models'][0])
		num_pca = len(data['scores'][0]) if data['scores'][0] != [1] else data.get('num_pca', 0)
		return {
			'img_dims': img_dims,
			'num_corr': num_corr,
			'num_pca': num_pca,
			'streaming': True
		}
	else:
		# Legacy format
		return {
			'img_dims': data.dataset.img[0].shape[1:],
			'num_corr': data.dataset.mdl_target[0].shape[0],
			'num_pca': data.dataset.pca_target[0].shape[0],
			'streaming': False
		}

'''
Reads csv and makes both train and validation data loaders from it
'''
def get_train_val_loaders(loader_dir, data_csv, batch_size=1, down_factor=1, down_dir=None, train_split=0.80, num_workers=0):
	sw_message("Creating training and validation torch loaders:")
	make_dir(loader_dir)
	images, scores, models, prefixes = get_all_train_data(loader_dir, data_csv, down_factor, down_dir)
	images, scores, models, prefixes = shuffle_data(images, scores, models, prefixes)
	# split into train and validation (e.g. 80% vs 20%)
	cut = int(len(images) * train_split)
	sw_message("Turning to tensors...")
	train_data = DeepSSMdataset(images[:cut], scores[:cut], models[:cut], prefixes[:cut])
	sw_message(str(len(train_data)) + ' in training set')
	val_data = DeepSSMdataset(images[cut:], scores[cut:], models[cut:], prefixes[cut:])
	sw_message(str(len(val_data)) + ' in validation set')

	sw_message("Saving data loaders...")
	trainloader = DataLoader(
			train_data,
			batch_size=batch_size,
			shuffle=True,
			num_workers=num_workers,
			pin_memory=torch.cuda.is_available()
		)
	train_path = loader_dir + C.TRAIN_LOADER
	torch.save(trainloader, train_path)

	validationloader = DataLoader(
			val_data,
			batch_size=1,
			shuffle=True,
			num_workers=num_workers,
			pin_memory=torch.cuda.is_available()
		)
	val_path = loader_dir + C.VALIDATION_LOADER
	torch.save(validationloader, val_path)
	sw_message("Training and validation loaders complete.\n")
	return train_path, val_path

'''
Reads csv and makes just train data loaders
'''
def get_train_loader(loader_dir, data_csv, batch_size=1, down_factor=1, down_dir=None, train_split=0.80, num_workers=0):
	sw_message("Creating training torch loader...")
	make_dir(loader_dir)

	if USE_STREAMING:
		# Streaming approach - don't load all images into memory
		image_paths, scores, models, prefixes = get_all_train_data_streaming(
			loader_dir, data_csv, down_factor, down_dir
		)
		image_paths, scores, models, prefixes = shuffle_data(image_paths, scores, models, prefixes)

		# Load saved mean/std
		mean_img = np.load(loader_dir + C.MEAN_IMG_FILE)
		std_img = np.load(loader_dir + C.STD_IMG_FILE)

		train_data = DeepSSMdatasetStreaming(
			list(image_paths), list(scores), list(models), list(prefixes),
			float(mean_img), float(std_img)
		)

		# For streaming, we don't save the full DataLoader (it would try to pickle the dataset)
		# Instead, save metadata that can be used to reconstruct the loader
		trainloader = DataLoader(
			train_data,
			batch_size=batch_size,
			shuffle=True,
			num_workers=num_workers,
			pin_memory=torch.cuda.is_available()
		)

		# Save metadata for reconstruction
		train_meta = {
			'image_paths': list(image_paths),
			'scores': list(scores),
			'models': list(models),
			'prefixes': list(prefixes),
			'mean_img': float(mean_img),
			'std_img': float(std_img),
			'batch_size': batch_size,
			'num_workers': num_workers,
			'streaming': True
		}
		train_path = loader_dir + C.TRAIN_LOADER
		torch.save(train_meta, train_path)
		sw_message("Training loader complete.")
		return train_path
	else:
		# Legacy approach - load all into memory
		images, scores, models, prefixes = get_all_train_data(loader_dir, data_csv, down_factor, down_dir)
		images, scores, models, prefixes = shuffle_data(images, scores, models, prefixes)
		train_data = DeepSSMdataset(images, scores, models, prefixes)
		trainloader = DataLoader(
			train_data,
			batch_size=batch_size,
			shuffle=True,
			num_workers=num_workers,
			pin_memory=torch.cuda.is_available()
		)
		train_path = loader_dir + C.TRAIN_LOADER
		torch.save(trainloader, train_path)
		sw_message("Training loader complete.")
		return train_path

'''
Makes validation data loader
'''
def get_validation_loader(loader_dir, val_img_list, val_particles, down_factor=1, down_dir=None, num_workers=0):
	sw_message("Creating validation torch loader:")
	if not val_img_list:
		raise DataLoadingError("Validation image list is empty")
	if not val_particles:
		raise DataLoadingError("Validation particle list is empty")
	if len(val_img_list) != len(val_particles):
		raise DataLoadingError(
			f"Mismatched validation data: {len(val_img_list)} images but {len(val_particles)} particle files"
		)

	# Get data
	image_paths = []
	scores = []
	models = []
	names = []
	for index in range(len(val_img_list)):
		image_path = val_img_list[index]
		prefix = get_prefix(image_path)
		names.append(prefix)
		image_paths.append(image_path)
		scores.append([1])  # placeholder
		mdl = get_particles(val_particles[index])
		models.append(mdl)

	# Write validation names to file
	name_file = open(loader_dir + C.VALIDATION_NAMES_FILE, 'w+')
	name_file.write(str(names))
	name_file.close()
	sw_message("Validation names saved to: " + loader_dir + C.VALIDATION_NAMES_FILE)

	if USE_STREAMING:
		# Prepare image paths
		image_paths = prepare_image_paths(image_paths, down_factor, down_dir)

		# Load mean/std from training (should already exist)
		mean_img = float(np.load(loader_dir + C.MEAN_IMG_FILE))
		std_img = float(np.load(loader_dir + C.STD_IMG_FILE))

		val_data = DeepSSMdatasetStreaming(image_paths, scores, models, names, mean_img, std_img)

		val_loader = DataLoader(
			val_data,
			batch_size=1,
			shuffle=False,
			num_workers=num_workers,
			pin_memory=torch.cuda.is_available()
		)

		# Save metadata
		val_meta = {
			'image_paths': image_paths,
			'scores': scores,
			'models': models,
			'names': names,
			'mean_img': mean_img,
			'std_img': std_img,
			'num_workers': num_workers,
			'streaming': True
		}
		val_path = loader_dir + C.VALIDATION_LOADER
		torch.save(val_meta, val_path)
		sw_message("Validation loader complete.")
		return val_path
	else:
		# Legacy approach
		images = get_images(loader_dir, image_paths, down_factor, down_dir)
		val_data = DeepSSMdataset(images, scores, models, names)
		val_loader = DataLoader(
			val_data,
			batch_size=1,
			shuffle=False,
			num_workers=num_workers,
			pin_memory=torch.cuda.is_available()
		)
		val_path = loader_dir + C.VALIDATION_LOADER
		torch.save(val_loader, val_path)
		sw_message("Validation loader complete.")
		return val_path

'''
Makes test data loader
'''
def get_test_loader(loader_dir, test_img_list, down_factor=1, down_dir=None, num_workers=0):
	sw_message("Creating test torch loader...")
	if not test_img_list:
		raise DataLoadingError("Test image list is empty")

	# Get data
	image_paths = []
	scores = []
	models = []
	test_names = []
	for index in range(len(test_img_list)):
		image_path = test_img_list[index]
		prefix = get_prefix(image_path)
		test_names.append(prefix)
		image_paths.append(image_path)
		scores.append([1])  # placeholder
		models.append([1])  # placeholder

	# Write test names to file
	name_file = open(loader_dir + C.TEST_NAMES_FILE, 'w+')
	name_file.write(str(test_names))
	name_file.close()
	sw_message("Test names saved to: " + loader_dir + C.TEST_NAMES_FILE)

	if USE_STREAMING:
		# Prepare image paths
		image_paths = prepare_image_paths(image_paths, down_factor, down_dir)

		# Load mean/std from training
		mean_img = float(np.load(loader_dir + C.MEAN_IMG_FILE))
		std_img = float(np.load(loader_dir + C.STD_IMG_FILE))

		test_data = DeepSSMdatasetStreaming(image_paths, scores, models, test_names, mean_img, std_img)

		testloader = DataLoader(
			test_data,
			batch_size=1,
			shuffle=False,
			num_workers=num_workers,
			pin_memory=torch.cuda.is_available()
		)

		# Save metadata
		test_meta = {
			'image_paths': image_paths,
			'scores': scores,
			'models': models,
			'names': test_names,
			'mean_img': mean_img,
			'std_img': std_img,
			'num_workers': num_workers,
			'streaming': True
		}
		test_path = loader_dir + C.TEST_LOADER
		torch.save(test_meta, test_path)
		sw_message("Test loader complete.")
		return test_path, test_names
	else:
		# Legacy approach
		images = get_images(loader_dir, image_paths, down_factor, down_dir)
		test_data = DeepSSMdataset(images, scores, models, test_names)
		testloader = DataLoader(
			test_data,
			batch_size=1,
			shuffle=False,
			num_workers=num_workers,
			pin_memory=torch.cuda.is_available()
		)
		test_path = loader_dir + C.TEST_LOADER
		torch.save(testloader, test_path)
		sw_message("Test loader complete.")
		return test_path, test_names

################################ Helper functions ######################################

'''
Returns image_paths, scores, models, prefixes from CSV for streaming.
Computes mean/std incrementally without loading all images.
'''
def get_all_train_data_streaming(loader_dir, data_csv, down_factor, down_dir):
	if not os.path.exists(data_csv):
		raise DataLoadingError(f"CSV file not found: {data_csv}")

	image_paths = []
	scores = []
	models = []
	prefixes = []

	try:
		with open(data_csv, newline='') as csvfile:
			datareader = csv.reader(csvfile)
			for row_num, row in enumerate(datareader, 1):
				if len(row) < 3:
					raise DataLoadingError(
						f"Invalid row {row_num} in {data_csv}: expected at least 3 columns "
						f"(image_path, model_path, pca_scores), got {len(row)}"
					)
				image_path = row[0]
				model_path = row[1]
				pca_scores = row[2:]

				prefix = get_prefix(image_path)
				prefixes.append(prefix)
				image_paths.append(image_path)

				try:
					pca_scores = [float(i) for i in pca_scores]
				except ValueError as e:
					raise DataLoadingError(
						f"Invalid PCA scores in {data_csv} at row {row_num}: {e}"
					)
				scores.append(pca_scores)

				mdl = get_particles(model_path)
				models.append(mdl)
	except csv.Error as e:
		raise DataLoadingError(f"Error parsing CSV file {data_csv}: {e}")

	if not image_paths:
		raise DataLoadingError(f"CSV file is empty: {data_csv}")

	# Prepare image paths (apply downsampling if needed)
	image_paths = prepare_image_paths(image_paths, down_factor, down_dir)

	# Compute mean/std incrementally
	sw_message("Computing image statistics incrementally...")
	mean_img, std_img = compute_image_stats_incremental(image_paths, down_factor, down_dir)
	np.save(loader_dir + C.MEAN_IMG_FILE, mean_img)
	np.save(loader_dir + C.STD_IMG_FILE, std_img)
	sw_message(f"Image stats: mean={mean_img:.4f}, std={std_img:.4f}")

	# Whiten PCA scores
	scores = whiten_PCA_scores(scores, loader_dir)

	return image_paths, scores, models, prefixes


'''
returns images, scores, models, prefixes from CSV (legacy - loads all into memory)
'''
def get_all_train_data(loader_dir, data_csv, down_factor, down_dir):
	if not os.path.exists(data_csv):
		raise DataLoadingError(f"CSV file not found: {data_csv}")
	# get all data and targets
	image_paths = []
	scores = []
	models = []
	prefixes = []
	try:
		with open(data_csv, newline='') as csvfile:
			datareader = csv.reader(csvfile)
			for row_num, row in enumerate(datareader, 1):
				if len(row) < 3:
					raise DataLoadingError(
						f"Invalid row {row_num} in {data_csv}: expected at least 3 columns "
						f"(image_path, model_path, pca_scores), got {len(row)}"
					)
				image_path = row[0]
				model_path = row[1]
				pca_scores = row[2:]
				# add name
				prefix = get_prefix(image_path)
				prefixes.append(prefix)
				# add image path
				image_paths.append(image_path)
				# add score (un-normalized)
				try:
					pca_scores = [float(i) for i in pca_scores]
				except ValueError as e:
					raise DataLoadingError(
						f"Invalid PCA scores in {data_csv} at row {row_num}: {e}"
					)
				scores.append(pca_scores)
				# add model
				mdl = get_particles(model_path)
				models.append(mdl)
	except csv.Error as e:
		raise DataLoadingError(f"Error parsing CSV file {data_csv}: {e}")

	if not image_paths:
		raise DataLoadingError(f"CSV file is empty: {data_csv}")

	images = get_images(loader_dir, image_paths, down_factor, down_dir)
	scores = whiten_PCA_scores(scores, loader_dir)
	return images, scores, models, prefixes 

'''
Shuffle all data
'''
def shuffle_data(images, scores, models, prefixes):
	c = list(zip(images, scores, models, prefixes))
	random.shuffle(c)
	images, scores, models, prefixes = zip(*c)
	return images, scores, models, prefixes

'''
Class for DeepSSM datasets that works with Pytorch DataLoader
Loads all images into memory upfront (legacy approach).
'''
class DeepSSMdataset():
	def __init__(self, img, pca_target, mdl_target, names):
		self.img = torch.FloatTensor(np.array(img))
		self.pca_target = torch.FloatTensor(np.array(pca_target))
		self.mdl_target = torch.FloatTensor(np.array(mdl_target))
		self.names = names
	def __getitem__(self, index):
		x = self.img[index]
		y1 = self.pca_target[index]
		y2 = self.mdl_target[index]
		name = self.names[index]
		return x, y1, y2, name
	def __len__(self):
		return len(self.img)


'''
Streaming dataset that loads images on-demand to minimize memory usage.
Only keeps file paths in memory, loads each image when accessed.
'''
class DeepSSMdatasetStreaming(Dataset):
	def __init__(self, image_paths, pca_target, mdl_target, names, mean_img, std_img):
		self.image_paths = image_paths
		self.pca_target = torch.FloatTensor(np.array(pca_target))
		self.mdl_target = torch.FloatTensor(np.array(mdl_target))
		self.names = names
		self.mean_img = mean_img
		self.std_img = std_img

	def __getitem__(self, index):
		# Load image on-demand
		image_path = self.image_paths[index]
		try:
			img = sw.Image(image_path).toArray(copy=True, for_viewing=True)
		except Exception as e:
			raise DataLoadingError(f"Error reading image {image_path}: {e}")

		# Normalize
		img = (img - self.mean_img) / self.std_img
		x = torch.FloatTensor(img).unsqueeze(0)  # Add channel dimension

		y1 = self.pca_target[index]
		y2 = self.mdl_target[index]
		name = self.names[index]
		return x, y1, y2, name

	def __len__(self):
		return len(self.image_paths)

'''
returns sample prefix from path string
'''
def get_prefix(path):
	file_name = os.path.basename(path)
	prefix = file_name.split(".")[0]
	return prefix

'''
get list from .particles format
'''
def get_particles(model_path):
	if not os.path.exists(model_path):
		raise DataLoadingError(f"Particle file not found: {model_path}")
	try:
		with open(model_path, "r") as f:
			data = []
			for line_num, line in enumerate(f.readlines(), 1):
				points = line.split()
				try:
					points = [float(i) for i in points]
				except ValueError as e:
					raise DataLoadingError(
						f"Invalid particle data in {model_path} at line {line_num}: {e}"
					)
				data.append(points)
		if not data:
			raise DataLoadingError(f"Particle file is empty: {model_path}")
		return data
	except IOError as e:
		raise DataLoadingError(f"Error reading particle file {model_path}: {e}")

'''
Compute image mean and std incrementally without loading all images into memory.
Uses Welford's online algorithm for numerical stability.
'''
def compute_image_stats_incremental(image_list, down_factor=1, down_dir=None):
	if not image_list:
		raise DataLoadingError("Image list is empty")

	n = 0
	mean = 0.0
	M2 = 0.0  # Sum of squared differences from mean

	for i, image_path in enumerate(image_list):
		# Handle downsampling
		if down_dir is not None:
			make_dir(down_dir)
			img_name = os.path.basename(image_path)
			res_img = os.path.join(down_dir, img_name)
			if not os.path.exists(res_img):
				apply_down_sample(image_path, res_img, down_factor)
			image_path = res_img

		if not os.path.exists(image_path):
			raise DataLoadingError(f"Image file not found: {image_path}")

		try:
			img = sw.Image(image_path).toArray(copy=True, for_viewing=True)
		except Exception as e:
			raise DataLoadingError(f"Error reading image {image_path}: {e}")

		# Welford's online algorithm for each pixel value
		for val in img.flat:
			n += 1
			delta = val - mean
			mean += delta / n
			delta2 = val - mean
			M2 += delta * delta2

		# Free memory
		del img

		if (i + 1) % 10 == 0:
			sw_message(f"  Computing stats: {i + 1}/{len(image_list)} images processed")

	if n < 2:
		raise DataLoadingError("Need at least 2 pixel values to compute statistics")

	variance = M2 / n
	std = np.sqrt(variance)

	return mean, std


'''
Prepare image paths, applying downsampling if needed.
Returns list of paths to use (either original or downsampled).
'''
def prepare_image_paths(image_list, down_factor=1, down_dir=None):
	if not image_list:
		raise DataLoadingError("Image list is empty")

	prepared_paths = []
	for image_path in image_list:
		if down_dir is not None:
			make_dir(down_dir)
			img_name = os.path.basename(image_path)
			res_img = os.path.join(down_dir, img_name)
			if not os.path.exists(res_img):
				apply_down_sample(image_path, res_img, down_factor)
			image_path = res_img

		if not os.path.exists(image_path):
			raise DataLoadingError(f"Image file not found: {image_path}")

		prepared_paths.append(image_path)

	return prepared_paths


'''
reads .nrrd files and returns whitened data (legacy - loads all into memory)
'''
def get_images(loader_dir, image_list, down_factor, down_dir):
	if not image_list:
		raise DataLoadingError("Image list is empty")
	# get all images
	all_images = []
	for image_path in image_list:
		if down_dir is not None:
			make_dir(down_dir)
			img_name = os.path.basename(image_path)
			res_img = os.path.join(down_dir, img_name)
			if not os.path.exists(res_img):
				apply_down_sample(image_path, res_img, down_factor)
			image_path = res_img
		if not os.path.exists(image_path):
			raise DataLoadingError(f"Image file not found: {image_path}")
		try:
			# for_viewing returns 'F' order, i.e., transpose, needed for this array
			img = sw.Image(image_path).toArray(copy=True, for_viewing=True)
		except Exception as e:
			raise DataLoadingError(f"Error reading image {image_path}: {e}")
		all_images.append(img)

	all_images = np.array(all_images)
	# get mean and std
	mean_path = loader_dir + C.MEAN_IMG_FILE
	std_path = loader_dir + C.STD_IMG_FILE
	mean_image = np.mean(all_images)
	std_image = np.std(all_images)
	np.save(mean_path, mean_image)
	np.save(std_path, std_image)
	# normalize
	norm_images = []
	for image in all_images:
		norm_images.append([(image-mean_image)/std_image])
	return norm_images

'''
decreases the size of the image 
'''
def apply_down_sample(image_path, output_path, factor=0.75):
	image = sw.Image(image_path)
	size = image.size()
	sizex = int(size[0]*factor)
	sizey = int(size[1]*factor)
	sizez = int(size[2]*factor)
	image.resize([sizex,sizey,sizez]).write(output_path)

'''
turns a string list into a vector
'''
def make_vector(str):
   arr = np.array(str.replace("[", "").replace("]", "").split(","))
   return np.asarray(arr, np.float64)

'''
normalizes PCA scores, returns mean and std for reconstruction
'''
def whiten_PCA_scores(scores, loader_dir):
	scores = np.array(scores)
	mean_score = np.mean(scores, 0)
	std_score = np.std(scores, 0)
	np.save(loader_dir + C.MEAN_PCA_FILE, mean_score)
	np.save(loader_dir + C.STD_PCA_FILE, std_score)
	norm_scores = []
	for score in scores:
		norm_scores.append((score-mean_score)/std_score)
	return norm_scores
