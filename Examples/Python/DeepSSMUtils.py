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

######################## Data Augmentation  ###############################

'''
Data augmentation helper
	Pads image by pad_amount
'''
def padImage(filename, pad_amount):
	[d, f] = nrrd.read(filename)
	dim = d.shape
	padamt = np.array([dim[0]+ pad_amount, dim[1] + pad_amount, dim[2] + pad_amount]) - dim
	x = np.floor(padamt/2)
	x = x.astype(np.int8)
	y = padamt - x
	y = y.astype(np.int8)
	dd = np.pad(d, ((x[0], y[0]), (x[1], y[1]), (x[2], y[2])), 'constant')
	return [dd, f]

'''
Data augmentation helper
	Down samples image by factor
'''
def downsampleNrrd(filename, factor):
	[inpt, f] = nrrd.read(filename)
	output = ndimage.interpolation.zoom(inpt, factor, prefilter=orig)
	return [np.array(output), f]

'''
Creates augmented images and particle models from provided list using PCA
Makes use of functions in DataAugmentUtils.py
Inputs:
	data_list - list of image paths
	point_list - list of corresponding .particles paths
	out_dir - folder to save data augmentation output
	num_samples - number of examples to generate
	PCA_var_cutoff - percent of variability used to decied how many PCA modes to retain (95-97% is usually a good range)
	doResample - if 1 generated and original images will be reampled to half their size for faster training
	doPad - if 1 a layer of padding will be added around generated and original images
Output:
	out_csv - a csv file with row containing: Path to image, path to particles, PCA modes
	(Note this contains both original and generated smaples in a shuffled order)
'''
def dataAugment(out_dir, data_list, point_list, num_samples, PCA_var_cutoff, doResample=0, doPad=0):
	if not os.path.exists(out_dir):
		os.makedirs(out_dir)
	N_images, M_particles, pt_dim, imgDims = ut.read_necessarry_metadata(data_list, point_list)
	K_pt, pca_particle_loadings, eigvals_particles, eigvecs_particles, mean_particles = ut.pca_mode_loading_computation(point_list, M_particles, N_images, pt_dim, out_dir, PCA_var_cutoff)
	np.save(os.path.join(out_dir, 'original_loadings_particles.npy'), pca_particle_loadings)
	print("The PCA modes of particles being retained : ", K_pt)

	print("\nWarping original images:")
	tilde_images_list = ut.warp_image_to_mean(point_list, data_list, out_dir)

	K_img, pca_images_loadings, eigvals_images, eigvecs_images, mean_image = ut.pca_mode_loadings_computation_images(tilde_images_list, N_images, imgDims, out_dir,PCA_var_cutoff)
	np.save(os.path.join(out_dir, 'original_loadings_images.npy'), pca_images_loadings)
	print("\nThe PCA modes of images being retained : ", K_img)

	print("\nGenerating particles:")
	generated_particles_list = ut.generate_particles(pca_particle_loadings, eigvals_particles, eigvecs_particles, mean_particles, num_samples, K_pt, M_particles, pt_dim, N_images, out_dir)
	
	print("\nGenerating images:")
	generated_tilde_images_list = ut.generate_images(pca_images_loadings, eigvals_images, eigvecs_images, mean_image, num_samples, K_img, imgDims, N_images, out_dir)

	print("\nWarping generated images:")
	generated_images_list = ut.warp_mean_to_image(generated_particles_list, generated_tilde_images_list, out_dir)

	# Paths lists
	image_path_list = data_list + generated_images_list
	particle_path_list = point_list + generated_particles_list

	# Get PCA loadings for csv
	# Get original loadings
	origLoadings = out_dir + 'original_loadings_particles.npy'
	K = np.load(origLoadings)
	num_orig = K.shape[0]
	K_pt = K.shape[1]
	# load the PCA stuff
	pcaDir = out_dir + 'PCA-Info-Particles/'
	mshp = np.loadtxt(pcaDir + 'meanshape.particles')
	N = mshp.shape[0]
	W = np.zeros([N*3, K_pt])
	for i in range(K_pt):
		pmd = np.loadtxt(pcaDir + 'pcamode' + str(i) + '.particles')
		W[..., i] = pmd.flatten()
	X = np.zeros([num_samples + num_orig, K_pt])
	particleDir = out_dir + 'Generated-Particles/'
	for i in range(num_samples):
		newScan = np.loadtxt(particleDir + 'Generated_sample_' + str(i) + '.particles')
		normed = newScan - mshp
		normed = normed.reshape([1, N*3])
		X[i, ...] = np.matmul(normed, W)
	X[num_samples:, ...] = K

	# Padding and resmapling for images
	print("\nResampling and padding if neccesary:")
	resmpleFactor = 0.5
	pad_amount = 4
	if doPad == 1:
		padDir = out_dir + 'paddedOut/'
		if not os.path.exists(padDir):
			os.makedirs(padDir)
	if doResample == 1:
		resampleDir = out_dir + 'resampleOut/'
		if not os.path.exists(resampleDir):
			os.makedirs(resampleDir)
	for i in range(len(image_path_list)):
		print("Image " + str(i) + " out of " + str(len(image_path_list)))
		if doPad == 1:
			[padout,f] = padImage(image_path_list[i], pad_amount)
			newnm = image_path_list[i].rsplit('/', 1)[1]
			newnm = newnm.replace('.nrrd', '.padded.nrrd')
			newnm = padDir + newnm
			nrrd.write(newnm, padout, f)
			image_path_list[i] = newnm
		if doResample == 1:
			[resampleout,f] = downsampleNrrd(image_path_list[i], resmpleFactor)
			newnm = image_path_list[i].rsplit('/', 1)[1]
			newnm = newnm.replace('.nrrd', '.resampled.nrrd')
			newnm = resampleDir + newnm
			nrrd.write(newnm, resampleout, f)
			image_path_list[i] = newnm

	# Write csv in shuffled order
	idx = np.arange(num_samples+num_orig)
	np.random.shuffle(idx)
	out_csv = out_dir + 'totalData.csv'
	with open(out_csv, 'w') as csvfile:
		spamwriter = csv.writer(csvfile, delimiter=",")
		for i in range(num_samples+num_orig):
			l = [image_path_list[idx[i]]]
			l.append(particle_path_list[idx[i]])
			for j in range(K_pt):
				l.append(X[idx[i], j])
			spamwriter.writerow(l)

	print("\nDone!")
	print(str(K_pt) +' PCA modes retained for particles.')
	print(str(K_img) + ' PCA modes retained for images.')
	print(str(num_samples) + ' samples generated from ' + str(num_orig) + " real examples.")
	return out_csv, pcaDir

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
	prefix = file_name.split(".")[0]
	prefix = prefix.replace("_1x_hip", "").replace("_femur", "")
	return prefix

'''
getTorchData helper
reads .nrrd file and retunrs data
the except is for the generated files, it forces the .nrrd to be in a format itk can read
'''
def getImage(path):
	image = itk.imread(path, itk.F)
	img = itk.GetArrayFromImage(image)
	return [list(img)]

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
Gets pytorch data loaders from data csv
Inputs:
	loader_dir - where to save data loaders
	data_csv - csv from data augmentation with rows or: image path, particles pat, pca scores
	batch_size - training batch size (memory dependent)
Output:
	train_path - path to train loader
	val_path - path to validation loader
	test_path - path to test loader
	test_names - list of names of files in test set
'''
def getTorchDataLoaders(loader_dir, data_csv, batch_size=1):
	if not os.path.exists(loader_dir):
		os.makedirs(loader_dir)
	# get all data and targets
	print("Reading all data...")
	images = []
	scores = []
	models = []
	prefixes = []
	with open(data_csv) as f:
		total = sum(1 for line in f)
	with open(data_csv, newline='') as csvfile:
		datareader = csv.reader(csvfile)
		index = 0
		for row in datareader:
			percent = ((index+1)/total) * 100
			if percent % 5 == 0:
				print(str(percent)+ '% processed')
			image_path = row[0]
			model_path = row[1]
			pca_scores = row[2:]
			# add name
			prefix = getPrefix(image_path)
			# data error check
			if prefix not in getPrefix(model_path):
				print("Error: Images and models mismatched in csv.")
				print(prefix)
				print(getPrefix(model_path))
				exit()
			prefixes.append(prefix)
			# add image
			img = getImage(image_path)
			images.append(img)
			# add score
			pca_scores = [float(i) for i in pca_scores]
			scores.append(pca_scores)
			# add model
			mdl = getParticles(model_path)
			models.append(mdl)
			index += 1

	print("\nShuffling and splitting into train, val, and test...")
	# shuffle
	c = list(zip(images, scores, models, prefixes))
	random.shuffle(c)
	images, scores, models, prefixes = zip(*c)
	# split into train (80%), validation(10%), and test(10%) datsets
	cut1 = int(len(images)*.8) 
	cut2 = cut1 + int(len(images)*.1)

	# Write test names to file so they are saved somewhere
	test_names = list(prefixes[cut2:])
	name_file = open(loader_dir + 'test_names.txt', 'w+')
	name_file.write(str(test_names))
	name_file.close()
	print("Test names saved to: " + loader_dir + "test_names.txt")

	print("\nTurning to tensors...")
	train_data = DeepSSMdataset(images[:cut1], scores[:cut1], models[:cut1])
	print(str(len(train_data)) + ' in training set')
	val_data = DeepSSMdataset(images[cut1:cut2], scores[cut1:cut2], models[cut1:cut2])
	print(str(len(val_data)) + ' in validation set')
	test_data = DeepSSMdataset(images[cut2:], scores[cut2:], models[cut2:])
	print(str(len(test_data)) + ' in testing set')

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
	testloader = DataLoader(
			test_data,
			batch_size=1,
			shuffle=False,
			num_workers=8,
			pin_memory=torch.cuda.is_available()
		)
	test_path = loader_dir + 'test'
	torch.save(testloader, test_path)
	print("Test loader done.")
	return train_path, val_path, test_path, test_names

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
def train(train_loader_path, validation_loader_path, parameters, parent_dir):
	# load le loaders
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
Test helper
	gets the original and predicted paricle coordinates from the pca scores
	saves them in out_dir
'''
def getPoints(out_dir, orig_scores, pred_scores, pca_score_path, test_names):
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
def test(out_dir, model_path, test_loader_path, test_names, pca_scores_path):
	# load le loaders
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
		index += 1
	test_mr_MSE = np.mean(np.sqrt(test_losses))
	test_rel_err =  np.mean(test_rel_losses)
	# Get paricles 
	orig_scores = np.array(orig_scores)
	pred_scores = np.array(pred_scores)
	getPoints(out_dir, orig_scores, pred_scores, pca_scores_path, test_names)

	return test_mr_MSE, test_rel_err

