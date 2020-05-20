import os
import numpy as np
from numpy import matlib
import itk
import dataAugmentUtils as ut
from collections import OrderedDict
import random
import time
import shutil
import subprocess
import nrrd
import torch
from torch import nn
from torch.nn import functional as F
from torch.utils.data import DataLoader

######################## Data Augmentation Functions ###############################

# def dataAugment(data_list, point_list, out_dir, num_samples, K_pt, K_img, th_pt, th_img, aug_type):
def dataAugment(data_list, point_list, out_dir, num_samples=100, K_pt=0, K_img=0, th_pt=1, th_img=0):
	if not os.path.exists(out_dir):
		os.makedirs(out_dir)
	N_images, M_particles, pt_dim, imgDims, f = ut.read_necessarry_metadata(data_list, point_list)
	K_pt, pca_particle_loadings, eigvals_particles, eigvecs_particles, mean_particles = ut.pca_mode_loading_computation(point_list, M_particles, N_images, pt_dim, out_dir, K_pt)
	np.save(os.path.join(out_dir, 'original_loadings_particles.npy'), pca_particle_loadings)
	print("The PCA modes of particles being retained : ", K_pt)
		
	tilde_images_list = ut.create_python_xml(point_list, data_list, out_dir)
	ut.create_cpp_xml("XML_convert_to_tilde_python.xml", "XML_convert_to_tilde_cpp.xml")
	print("Done")
	ut.warp_image_to_space("XML_convert_to_tilde_cpp.xml")
	
	K_img, pca_images_loadings, eigvals_images, eigvecs_images, mean_image = ut.pca_mode_loadings_computation_images(tilde_images_list, N_images, imgDims, out_dir, K_img, f)
	np.save(os.path.join(out_dir, 'original_loadings_images.npy'), pca_images_loadings)
	print("The PCA modes of images being retained : ", K_img)

	ut.generate_particles(pca_particle_loadings, eigvals_particles, eigvecs_particles, mean_particles, num_samples, K_pt, M_particles, pt_dim, N_images, th_pt, out_dir)

	ut.generate_images(pca_images_loadings, eigvals_images, eigvecs_images, mean_image, num_samples, K_img, imgDims, N_images, th_img, out_dir, f)
	ut.create_final_xml(num_samples, out_dir)
	ut.create_cpp_xml("XML_get_final_images_python.xml", "XML_get_final_images_cpp.xml")
	ut.warp_image_to_space("XML_get_final_images_cpp.xml")

	# if aug_type == "shape":
	# 	ut.generate_particles(pca_particle_loadings, eigvals_particles, eigvecs_particles, mean_particles, num_samples, K_pt, M_particles, pt_dim, N_images, th_pt, parent_dir)
	# 	ut.create_final_xml_shape(num_samples, parent_dir, point_list_array, data_list_array)
	# 	ut.create_cpp_xml("XML_get_final_images_python.xml", "XML_get_final_images_cpp.xml")
	# 	ut.warp_image_to_space("XML_get_final_images_cpp.xml")
	return [], [], ''

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
reads .nrrd file and retunrs data
the except is for the generated files, it forces the .nrrd to be in a format itk can read
'''
def getImage(path):
	print(path)
	try:
		image = itk.imread(path)
	except:
		image = itk.imread(path, itk.F)
	img = itk.GetArrayFromImage(image)
	return [list(img)]

def getImage2(path):
	img, f = nrrd.read(path)
	return [list(img)]

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
def getTorchDataLoaders(image_dir, model_dir, pca_file, parent_dir, batch_size=1):
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
	prefixes = []
	for index in range(len(imglist)):
	# for index in range(8,40):
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
		prefixes.append(prefix)
		# add image
		img = getImage(image_path)
		images.append(img)
		# add score
		score = PCAdict[prefix]
		scores.append(score)
		# add model
		mdl = getParticles(model_path)
		models.append(mdl)

	print("Shuffling and turning to tensors...")
	# shuffle
	c = list(zip(images, scores, models, prefixes))
	random.shuffle(c)
	images, scores, models, prefixes = zip(*c)
	# split into train (80%), validation(10%), and test(10%) datsets
	cut1 = int(len(images)*.8) 
	cut2 = cut1 + int(len(images)*.1)
	train_data = DeepSSMdataset(images[:cut1], scores[:cut1], models[:cut1])
	val_data = DeepSSMdataset(images[cut1:cut2], scores[cut1:cut2], models[cut1:cut2])
	test_data = DeepSSMdataset(images[cut2:], scores[cut2:], models[cut2:])
	test_names = prefixes[cut2:]

	print("Creating and saving dataloaders...")
	loader_dir = parent_dir + "TorchDataLoaders/"
	if not os.path.exists(loader_dir):
		os.makedirs(loader_dir)
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
	return train_path, val_path, test_path, test_names

########################## Model ####################################

class DeepSSMNet(nn.Module):
	def __init__(self):
		super(DeepSSMNet, self).__init__()
		if torch.cuda.is_available():
			device = 'cuda:0'
		else:
			device = 'cpu'
		self.device = device
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
			
			('fc1', nn.Linear(76800, 384)),
			('relu6', nn.PReLU()),
			('fc2', nn.Linear(384,96)),
			('relu7', nn.PReLU()),
		]))
		self.pca_pred = nn.Sequential(OrderedDict([
			('linear', nn.Linear(96, 6))
		]))
		# self.particles_pred = nn.Sequential(OrderedDict([
		# 	('linear', nn.Linear(6, 10000))
		# ]))
	def forward(self, x):
		x = self.features(x)
		return self.pca_pred(x)

class Flatten(nn.Module):
	def forward(self, x):
		return x.view(x.size(0), -1)

def weight_init(module, initf):
	def foo(m):
		classname = m.__class__.__name__.lower()
		if isinstance(m, module):
			initf(m.weight)
	return foo
#####################################

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

def train(model, train_loader_path, validation_loader_path, parameters, parent_dir):
	# initalizations
	num_epochs = parameters['epochs']
	learning_rate = parameters['learning_rate']
	eval_freq = parameters['val_freq']
	device = model.device
	model.cuda()
	# load le loaders
	print("Loading data loaders...")
	train_loader = torch.load(train_loader_path)
	val_loader = torch.load(validation_loader_path)
	print("Done.")
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


def test(model, test_loader_path, test_names):
	# initalizations
	device = model.device
	model.cuda()
	# load le loaders
	print("Loading test data loader...")
	test_loader = torch.load(test_loader_path)
	print("Done.\n")
	model.eval()
	test_losses = []
	test_rel_losses = []
	index = 0
	for img, pca, mdl in test_loader:
		img = img.to(device)
		pca = pca.to(device)
		pred = model(img)
		loss = torch.mean((pred - pca)**2)
		test_losses.append(loss.item())
		test_rel_loss = F.mse_loss(pred, pca) / F.mse_loss(pred*0, pca)
		test_rel_losses.append(test_rel_loss.item())
		index += 1
	test_mr_MSE = np.mean(np.sqrt(test_losses))
	test_rel_err =  np.mean(test_rel_losses)
	return test_mr_MSE, test_rel_err