import os
import numpy as np
import nrrd #@TODO not in conda installs (python -m pip install pynrrd)
import random
import torch
from torch.utils.data import DataLoader

######################## Data loading functions ####################################

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
getTorchData helper
returns sample prefix from path string
'''
def getPrefix(path):
	file_name = os.path.basename(path)
	prefix = '_'.join(file_name.split('_')[0:3])
	prefix.replace('.particles','')
	return prefix

'''
getTorchData helper
returns a dictionary of PCA scores from csv
{prefix:[pca scores]}
'''
def getPCAdictionary(pca_file):
	PCAdict = {}
	f = open(pca_file, "r")
	for line in f.readlines():
		line_array = line.replace('\n','').split(',')
		prefix = getPrefix(line_array[0])
		scores = [float(i) for i in line_array[1:]]
		PCAdict[prefix] = scores
	return PCAdict

'''
getTorchData helper
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
Gets pytorch data from image dir, model dir, and pca scores
Returns dir where train, validation, and test data loader are saved
'''
def getTorchDataLoaders(image_dir, model_dir, pca_file, parent_dir):
	print("Reading in data... \nPercent complete:")
	# get lists of file paths for images and models
	imglist = []
	for file in sorted(os.listdir(image_dir)):
		imglist.append(image_dir + file)
	modelList = []
	for file in sorted(os.listdir(model_dir)):
		modelList.append(model_dir + file)
	# get dictionary of PCA scores
	PCAdict = getPCAdictionary(pca_file)
	# data error check
	if len(imglist) != len(modelList) or len(imglist) != len(PCAdict.keys()):
		print("Error: inconstent number of images, models, and PCA scores.")
		exit()
	# get all data and targets
	images = []
	scores = []
	models = []
	# for index in range(len(imglist)):
	for index in range(10):
		if index%200 == 0:
			print(str((index/len(imglist))*100)+'%')
		image_path = imglist[index]
		prefix = getPrefix(image_path)
		model_path = ''
		for modelPath in modelList:
			if prefix in modelPath:
				model_path = modelPath
		# data error check
		if prefix not in getPrefix(model_path):
			print("Error: Images and models mismatched")
			print(prefix)
			exit()
		# add image
		[img,f] = nrrd.read(image_path)
		images.append(img)
		# add score
		score = PCAdict[prefix]
		scores.append(score)
		# add model
		mdl = getParticles(model_path)
		models.append(mdl)

	print("Shuffling and turning to tensors...")
	# shuffle
	c = list(zip(images, scores, models))
	random.shuffle(c)
	images, scores, models = zip(*c)
	# split into train (80%), validation(10%), and test(10%) datsets
	cut1 = int(len(images)*.8) 
	cut2 = cut1 + int(len(images)*.1)
	train_data = DeepSSMdataset(images[:cut1], scores[:cut1], models[:cut1])
	val_data = DeepSSMdataset(images[cut1:cut2], scores[cut1:cut2], models[cut1:cut2])
	test_data = DeepSSMdataset(images[cut2:], scores[cut2:], models[cut2:])

	print("Creating and saving dataloaders...")
	loader_dir = parent_dir + "TorchDataLoaders/"
	if not os.path.exists(loader_dir):
		os.makedirs(loader_dir)
	trainloader = DataLoader(
			train_data,
			batch_size=4,
			shuffle=True,
			num_workers=2,
			pin_memory=torch.cuda.is_available()
		)
	train_path = loader_dir + 'train'
	torch.save(trainloader, train_path)
	validationloader = DataLoader(
			val_data,
			batch_size=1,
			shuffle=True,
			num_workers=2,
			pin_memory=torch.cuda.is_available()
		)
	val_path = loader_dir + 'validation'
	torch.save(trainloader, val_path)
	testloader = DataLoader(
			test_data,
			batch_size=1,
			shuffle=True,
			num_workers=2,
			pin_memory=torch.cuda.is_available()
		)
	test_path = loader_dir + 'test'
	torch.save(trainloader, test_path)
	print("Done.")
	return train_path, val_path, test_path

##################### #################################
	def train(train_loader, val_loader):
		pass

	
