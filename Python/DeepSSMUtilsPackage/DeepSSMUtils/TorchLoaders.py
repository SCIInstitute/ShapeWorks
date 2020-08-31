import os
import numpy as np
import itk
import csv
import random
import subprocess
import torch
from torch import nn
from torch.utils.data import DataLoader

######################## Data loading functions ####################################

'''
Reads csv and makes train and validation data loaders
'''
def getTrainValLoaders(loader_dir, data_csv, batch_size=1, down_sample=False):
	if not os.path.exists(loader_dir):
		os.makedirs(loader_dir)
	images, scores, models, prefixes = getAllTrainData(loader_dir, data_csv, down_sample)
	images, scores, models, prefixes = shuffleData(images, scores, models, prefixes)
	# split into train (80%) validation(20%)
	cut = int(len(images)*.80) 
	print("\nTurning to tensors...")
	train_data = DeepSSMdataset(images[:cut], scores[:cut], models[:cut])
	print(str(len(train_data)) + ' in training set')
	val_data = DeepSSMdataset(images[cut:], scores[cut:], models[cut:])
	print(str(len(val_data)) + ' in validation set')

	print("\nCreating and saving dataloaders...")
	trainloader = DataLoader(
			train_data,
			batch_size=batch_size,
			shuffle=True,
			num_workers=8,
			pin_memory=torch.cuda.is_available()
		)
	train_path = loader_dir + 'train'
	torch.save(trainloader, train_path)
	print("Train loader done.")

	validationloader = DataLoader(
			val_data,
			batch_size=1,
			shuffle=True,
			num_workers=8,
			pin_memory=torch.cuda.is_available()
		)
	val_path = loader_dir + 'validation'
	torch.save(validationloader, val_path)
	print("Val loader done.")
	return train_path, val_path

'''
Makes test data loader
'''
def getTestLoader(loader_dir, test_img_list, test_particle_list, down_sample):
	# get data
	image_paths = []
	scores = []
	models = []
	test_names = []
	for index in range(len(test_img_list)):
		image_path = test_img_list[index]
		model_path = test_particle_list[index]
		# add name
		prefix = getPrefix(image_path)
		# data error check
		if prefix not in getPrefix(model_path):
			print("Error: Images and models mismatched in csv.")
			print(index)
			print(prefix)
			print(getPrefix(model_path))
			exit()
		test_names.append(prefix)
		image_paths.append(image_path)
		# add scoe placeholder
		scores.append([])
		models.append(getParticles(model_path))
	images = getImages(loader_dir, image_paths, down_sample)
	scores = whitenPCAscores(scores, loader_dir)

	test_data = DeepSSMdataset(images, scores, models)
	# Write test names to file so they are saved somewhere
	name_file = open(loader_dir + 'test_names.txt', 'w+')
	name_file.write(str(test_names))
	name_file.close()
	print("Test names saved to: " + loader_dir + "test_names.txt")
	# Make loader
	print("Creating and saving test dataloader...")
	testloader = DataLoader(
			test_data,
			batch_size=1,
			shuffle=False,
			num_workers=8,
			pin_memory=torch.cuda.is_available()
		)
	test_path = loader_dir + 'test'
	torch.save(testloader, test_path)
	print("Done.")
	return test_path, test_names

'''
returns images, scores, models, prefixes from CSV
'''
def getAllTrainData(loader_dir, data_csv, down_sample):
	# get all data and targets
	print("Reading all data...")
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
			prefix = getPrefix(image_path)
			# data error check
			if prefix not in getPrefix(model_path):
				print("Error: Images and models mismatched in csv.")
				print(index)
				print(prefix)
				print(getPrefix(model_path))
				exit()
			prefixes.append(prefix)
			# add image path
			image_paths.append(image_path)
			# add score (un-normalized)
			pca_scores = [float(i) for i in pca_scores]
			scores.append(pca_scores)
			# add model
			mdl = getParticles(model_path)
			models.append(mdl)
			index += 1
	images = getImages(loader_dir, image_paths, down_sample)
	scores = whitenPCAscores(scores, loader_dir)
	return images, scores, models, prefixes 

'''
Shuffle all data
'''
def shuffleData(images, scores, models, prefixes):
	print("Shuffling.")
	c = list(zip(images, scores, models, prefixes))
	random.shuffle(c)
	images, scores, models, prefixes = zip(*c)
	return images, scores, models, prefixes

'''
Class for DeepSSM datasets that works with Pytorch DataLoader
'''
class DeepSSMdataset():
	def __init__(self, img, pca_target, mdl_target):
		self.img = torch.FloatTensor(img)
		self.pca_target = torch.FloatTensor(pca_target)
		self.mdl_target = torch.FloatTensor(mdl_target)
	def __getitem__(self, index):
		x = self.img[index]
		y1 = self.pca_target[index]
		y2 = self.mdl_target[index]
		return x, y1, y2
	def __len__(self):
		return len(self.img)

'''
getTorchDataLoaderHelper
returns sample prefix from path string
'''
def getPrefix(path):
	file_name = os.path.basename(path)
	prefix = "_".join(file_name.split("_")[:2])
	prefix = prefix.split(".")[0]
	return prefix

'''
getTorchDataLoaderHelper
get list from .particles format
'''
def getParticles(model_path):
	f = open(model_path, "r")
	data = []
	for line in f.readlines():
		points = line.replace(' \n','').split(" ")
		points = [float(i) for i in points]
		data.append(points)
	return(data)

'''
getTorchDataLoaderHelper
reads .nrrd files and returns whitened data
'''
def getImages(loader_dir, image_list, down_sample):
	# get all images
	all_images = []
	for image_path in image_list:
		if down_sample:
			img = downSample(image_path)
		else:
			image = itk.imread(image_path, itk.F)
			img = itk.GetArrayFromImage(image)
		all_images.append(img)
	all_images = np.array(all_images)
	# get mean and std
	mean_path = loader_dir + 'mean_img.npy'
	std_path = loader_dir + 'std_img.npy'
	if not os.path.exists(mean_path) or not os.path.exists(std_path):
		mean_image = np.mean(all_images)
		std_image = np.std(all_images)
		np.save(mean_path, mean_image)
		np.save(std_path, std_image)
	else:
		mean_image = np.load(mean_path)
		std_image = np.load(std_path)
	# normlaize
	norm_images = []
	for image in all_images:
		norm_images.append([(image-mean_image)/std_image])
	return norm_images

'''
Halves the size of the image
'''
def downSample(image_path):
	path = os.path.dirname(image_path)
	temp_path = path + "/temp.nrrd"
	cmd = ["shapeworks",
		   "read-image", "--name", image_path,
		   "info", "--size", str(True)]
	output = subprocess.run(cmd, capture_output=True, text=True).stdout.splitlines()
	size = makeVector(output[0].split(":")[1])
	sizex = int(size[0]/2)
	sizey = int(size[1]/2)
	sizez = int(size[2]/2)
	cmd = ["shapeworks", 
		"read-image", "--name", image_path,
		"resize", "--sizex", str(sizex),
		"--sizey", str(sizey),
		"--sizez", str(sizez),
		"write-image", "--name", temp_path]
	subprocess.check_call(cmd)
	image = itk.imread(temp_path, itk.F)
	img = itk.GetArrayFromImage(image)
	os.remove(temp_path)
	return img
 
def makeVector(str):
   arr = np.array(str.replace("[", "").replace("]", "").split(","))
   return np.asarray(arr, np.float64)

'''
getTorchDataLoaderHelper
normalizes PCA scores, returns mean and std for reconstruction
'''
def whitenPCAscores(scores, loader_dir):
	scores = np.array(scores)
	mean_score = np.mean(scores)
	std_score = np.std(scores)
	np.save(loader_dir + 'mean_PCA.npy', mean_score)
	np.save(loader_dir + 'std_PCA.npy', std_score)
	norm_scores = []
	for score in scores:
		norm_scores.append((score-mean_score)/std_score)
	return norm_scores