# Jadie Adams
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
			
			('fc1', nn.Linear(17280, 384)),
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
	print("Defining model...")
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
	print("Done.")
	# train
	print("Beginning training on device = " + device + '\n')
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
	gets the predicted paricle coordinates from the pca scores
	saves them in out_dir
'''
def getPoints(predPath, pred_scores, pca_score_path, test_names, loader_dir):
	print("Getting particles from predicted PCA scores...")
	mean_PCA = np.load(loader_dir + 'mean_PCA.npy')
	std_PCA = np.load(loader_dir + 'std_PCA.npy')
	pred_scores = undoNorm(pred_scores, mean_PCA, std_PCA)
	if not os.path.exists(predPath):
		os.makedirs(predPath)
	N = pred_scores.shape[0]
	K = pred_scores.shape[1]
	# now create the PCA matrix
	meanshape = np.loadtxt(pca_score_path + '/mean.particles')
	M = meanshape.shape[0]
	W = np.zeros([K, 3*M])
	for i in range(K):
		nm = pca_score_path + '/pcamode' + str(i) + '.particles'
		prt = np.loadtxt(nm)
		W[i, ...] = prt.flatten()
	pointsPred = np.matmul(pred_scores,W) + np.matlib.repmat(meanshape.reshape(1, 3*M), N, 1)

	for i in range(N):
		print("Data Point : ", i)
		nmpred = predPath + 'predicted_' + test_names[i] + '.particles'
		tmpPred = pointsPred[i, ...].reshape(M, 3)
		np.savetxt(nmpred, tmpPred)
	print("Done.\n")

'''
Network Test Function
	predicts the PCA scores using the trained networks
	returns the error measures and saves the predicted and poriginal particles for comparison
'''
def test(out_dir, model_path, loader_dir, pca_scores_path, num_pca):
	if not os.path.exists(out_dir):
		os.makedirs(out_dir)
	# load le loaders
	test_loader_path = loader_dir + "test"
	print("Loading test data loader...")
	test_loader = torch.load(test_loader_path)
	print("Done.\n")
	# initalizations
	print("Loading trained model...")
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
	print("Done.\n")
	print("Predicting for test images...")
	index = 0
	pred_scores = []
	for img, pca, mdl in test_loader:
		img = img.to(device)
		pred = model(img)
		pred_scores.append(pred.cpu().data.numpy()[0])
		index += 1
		print("Predicted test " + str(index) + ".")
	print("Done.\n")
	pred_scores = np.array(pred_scores)
	getPoints(out_dir, pred_scores, pca_scores_path, test_names, loader_dir)
	return

'''
Make folder
'''
def makeDir(dirPath):
    if not os.path.exists(dirPath):
        os.makedirs(dirPath)