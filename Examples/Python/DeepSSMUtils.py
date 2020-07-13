import DataAugmentUtils as ut
import os
import numpy as np
from numpy import matlib
import itk
import csv
from scipy import ndimage
from collections import OrderedDict
import random
import time
import shutil
import subprocess
import torch
from torch import nn
from torch.nn import functional as F
from torch.utils.data import DataLoader

######################## Data loading functions ####################################

'''
Reads csv and makes train and validation data loaders
'''
def getTrainValLoaders(loader_dir, data_csv, batch_size=1, down_sample=False):
	makeDir(loader_dir)
	images, scores, models, prefixes = getAllTrainData(data_csv, down_sample)
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
	images = getImages(image_paths, down_sample)
	scores = whitenPCAscores(scores, loader_dir, mean_std_dir)

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
def getAllTrainData(data_csv, down_sample):
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
	scores = whitenPCAscores(scores, loader_dir, mean_std_dir)
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
		image = itk.imread(image_path, itk.F)
		img = itk.GetArrayFromImage(image)
		if down_sample:
			img = downSample(img)
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

# @TODO
def downSample(img):
	return img

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


########################## Model Class ####################################

'''
Vanilla DeepSSM Model
'''
class DeepSSMNet(nn.Module):
	def __init__(self, num_pca):
		super(DeepSSMNet, self).__init__()
		if torch.cuda.is_available():
			device = 'cuda:0'
		else:
			device = 'cpu'
		self.device = device
		self.num_pca = num_pca
		self.features = nn.Sequential(OrderedDict([
			('conv1', nn.Conv3d(1, 12, 5)),
			('bn1', nn.BatchNorm3d(12)),
			('relu1', nn.PReLU()),
			('mp1', nn.MaxPool3d(2)),

			('conv2', nn.Conv3d(12, 24, 5)),
			('bn2', nn.BatchNorm3d(24)),
			('relu2', nn.PReLU()),
			('conv3', nn.Conv3d(24, 48, 5)),
			('bn3', nn.BatchNorm3d(48)),
			('relu3', nn.PReLU()),
			('mp2', nn.MaxPool3d(2)),

			('conv4', nn.Conv3d(48, 96, 5)),
			('bn4', nn.BatchNorm3d(96)),
			('relu4', nn.PReLU()),
			('conv5', nn.Conv3d(96, 192, 5)),
			('bn5', nn.BatchNorm3d(192)),
			('relu5', nn.PReLU()),
			('mp3', nn.MaxPool3d(2)),

			('flatten', Flatten()),
			
			('fc1', nn.Linear(103680, 384)),
			('relu6', nn.PReLU()),
			('fc2', nn.Linear(384,96)),
			('relu7', nn.PReLU()),
		]))
		self.pca_pred = nn.Sequential(OrderedDict([
			('linear', nn.Linear(96, self.num_pca))
		]))
	def forward(self, x):
		x = self.features(x)
		return self.pca_pred(x)

class Flatten(nn.Module):
	def forward(self, x):
		return x.view(x.size(0), -1)

##################################### Train Functions ###########################

'''
Train helper
	Initilaizes all weights using initialization function specified by initf
'''
def weight_init(module, initf):
	def foo(m):
		classname = m.__class__.__name__.lower()
		if isinstance(m, module):
			initf(m.weight)
	return foo

'''
Train helper
	prints and logs values during training
'''
def log_print(logger, values):
	# print values
	if isinstance(values[0], str):
		print('	  '.join(values))
	else:
		format_values = ['%.4f' % i for i in values]
		print('	  '.join(format_values))
	# csv format
	string_values = [str(i) for i in values]
	log_string = ','.join(string_values)
	logger.write(log_string + '\n')

'''
Network training method
	defines, initializes, and trains the models
	logs training and validation errors
	saves the model and returns the path it is saved to
'''
def train(loader_dir, parameters, parent_dir):
	# load le loaders
	train_loader_path = loader_dir + "train"
	validation_loader_path = loader_dir + "validation"
	print("Loading data loaders...")
	train_loader = torch.load(train_loader_path)
	val_loader = torch.load(validation_loader_path)
	print("Done.")
	# initalizations
	num_pca = train_loader.dataset.pca_target[0].shape[0]
	print("Defining model.")
	model = DeepSSMNet(num_pca)
	device = model.device
	model.cuda()
	num_epochs = parameters['epochs']
	learning_rate = parameters['learning_rate']
	eval_freq = parameters['val_freq']
	# intialize model weights
	model.apply(weight_init(module=nn.Conv2d, initf=nn.init.xavier_normal_))	
	model.apply(weight_init(module=nn.Linear, initf=nn.init.xavier_normal_))
	# define optimizer
	train_params = model.parameters()
	opt = torch.optim.Adam(train_params, learning_rate)
	opt.zero_grad()
	# train
	print("Beginning training on device = " + device)
	logger = open(parent_dir + "train_log.csv", "w+")
	log_print(logger, ["Epoch", "Train_Err", "Train_Rel_Err", "Val_Err", "Val_Rel_Err", "Sec"])
	t0 = time.time()
	for e in range(1, num_epochs + 1):
		torch.cuda.empty_cache()
		# train
		model.train()
		train_losses = []
		train_rel_losses = []
		for img, pca, mdl in train_loader:
			opt.zero_grad()
			img = img.to(device)
			pca = pca.to(device)
			pred = model(img)
			loss = torch.mean((pred - pca)**2)
			loss.backward()
			opt.step()
			train_losses.append(loss.item())
			train_rel_loss = F.mse_loss(pred, pca) / F.mse_loss(pred*0, pca)
			train_rel_losses.append(train_rel_loss.item())
		# test validation
		if ((e % eval_freq) == 0 or e == 1):
			model.eval()
			val_losses = []
			val_rel_losses = []
			for img, pca, mdl in val_loader:
				opt.zero_grad()
				img = img.to(device)
				pca = pca.to(device)
				pred = model(img)
				v_loss = torch.mean((pred - pca)**2)
				val_losses.append(v_loss.item())
				val_rel_loss = F.mse_loss(pred, pca) / F.mse_loss(pred*0, pca)
				val_rel_losses.append(val_rel_loss.item())
			# log
			train_mr_MSE = np.mean(np.sqrt(train_losses))
			val_mr_MSE = np.mean(np.sqrt(val_losses))
			train_rel_err = np.mean(train_rel_losses)
			val_rel_err =  np.mean(val_rel_losses)
			log_print(logger, [e, train_mr_MSE, train_rel_err, val_mr_MSE, val_rel_err, time.time()-t0])
			# save
			torch.save(model.state_dict(), os.path.join(parent_dir, 'model.torch'))
			torch.save(opt.state_dict(), os.path.join(parent_dir, 'opt.torch'))
			t0 = time.time()
	# save
	logger.close()
	torch.save(model.state_dict(), os.path.join(parent_dir, 'model.torch'))
	torch.save(opt.state_dict(), os.path.join(parent_dir, 'opt.torch'))
	print("Training complete, model saved.")
	return os.path.join(parent_dir, 'model.torch')

############################## Test Model #################################
'''
undo data whitening
''' 
def undoNorm(data_list, mean, std):
	unnorm_data = []
	for data in data_list:
		unnorm_data.append((data*std)+mean)
	return np.array(unnorm_data)

'''
Test helper
	gets the original and predicted paricle coordinates from the pca scores
	saves them in out_dir
'''
def getPoints(out_dir, orig_scores, pred_scores, pca_score_path, test_names, loader_dir):
	mean_PCA = np.load(loader_dir + 'mean_PCA.npy')
	std_PCA = np.load(loader_dir + 'std_PCA.npy')
	orig_scores = undoNorm(orig_scores, mean_PCA, std_PCA)
	pred_scores = undoNorm(pred_scores, mean_PCA, std_PCA)
	if not os.path.exists(out_dir):
		os.makedirs(out_dir)
	predPath = out_dir + 'predictedPoints/'
	if not os.path.exists(predPath):
		os.makedirs(predPath)
	origPath = out_dir + 'originalPoints/'
	if not os.path.exists(origPath):
		os.makedirs(origPath)
	N = orig_scores.shape[0]
	K = orig_scores.shape[1]
	# now create the PCA matrix
	meanshape = np.loadtxt(pca_score_path + '/meanshape.particles')
	M = meanshape.shape[0]
	W = np.zeros([K, 3*M])
	for i in range(K):
		nm = pca_score_path + '/pcamode' + str(i) + '.particles'
		prt = np.loadtxt(nm)
		W[i, ...] = prt.flatten()
	pointsPred = np.matmul(pred_scores,W) + np.matlib.repmat(meanshape.reshape(1, 3*M), N, 1)
	pointsOrig = np.matmul(orig_scores,W) + np.matlib.repmat(meanshape.reshape(1, 3*M), N, 1)

	for i in range(N):
		print("Data Point : ", i)
		nmpred = predPath + 'predicted_' + test_names[i] + '.particles'
		nmorig = origPath + 'original_' + test_names[i] + '.particles'
		tmpPred = pointsPred[i, ...].reshape(M, 3)
		tmpOrig = pointsOrig[i, ...].reshape(M, 3)
		np.savetxt(nmpred, tmpPred)
		np.savetxt(nmorig, tmpOrig)

'''
Network Test Function
	predicts the PCA scores using the trained networks
	returns the error measures and saves the predicted and poriginal particles for comparison
'''
def test(out_dir, model_path, loader_dir, pca_scores_path):
	if not os.path.exists(out_dir):
		os.makedirs(out_dir)
	# load le loaders
	test_loader_path = loader_dir + "test"
	print("Loading test data loader...")
	test_loader = torch.load(test_loader_path)
	print("Done.\n")
	# initalizations
	num_pca = test_loader.dataset.pca_target[0].shape[0]
	model = DeepSSMNet(num_pca)
	model.load_state_dict(torch.load(model_path))
	device = model.device
	model.cuda()
	model.eval()
	# Get test names 
	test_names_file = loader_dir + 'test_names.txt'
	f = open(test_names_file, 'r')
	test_names_string = f.read()
	f.close()
	test_names_string = test_names_string.replace("[","").replace("]","").replace("'","")
	test_names = test_names_string.split(",")
	# Logger
	logger = open(out_dir + "results.csv", "w+")
	log_print(logger, ["Name", "Test_Err", "Test_Rel_Err"])
	# Get predicted PCA scores and errors
	test_losses = []
	test_rel_losses = []
	index = 0
	orig_scores = []
	pred_scores = []
	for img, pca, mdl in test_loader:
		img = img.to(device)
		pca = pca.to(device)
		pred = model(img)
		orig_scores.append(pca.cpu().data.numpy()[0])
		pred_scores.append(pred.cpu().data.numpy()[0])
		loss = torch.mean((pred - pca)**2)
		test_losses.append(loss.item())
		test_rel_loss = F.mse_loss(pred, pca) / F.mse_loss(pred*0, pca)
		test_rel_losses.append(test_rel_loss.item())
		log_print(logger, [test_names[index], str(loss.item()), str(test_rel_loss.item())])
		index += 1
	logger.close()
	test_mr_MSE = np.mean(np.sqrt(test_losses))
	test_rel_err =  np.mean(test_rel_losses)
	# Get paricles 
	orig_scores = np.array(orig_scores)
	pred_scores = np.array(pred_scores)
	getPoints(out_dir, orig_scores, pred_scores, pca_scores_path, test_names, loader_dir)
	return test_mr_MSE, test_rel_err

'''
Make folder
'''
def makeDir(dirPath):
    if not os.path.exists(dirPath):
        os.makedirs(dirPath)