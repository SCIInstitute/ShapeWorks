import torch
from torch import nn
import sys
import json
import numpy as np
from collections import OrderedDict
from DeepSSMUtils import net_utils


class ConvolutionalBackbone(nn.Module):
	def __init__(self, img_dims):
		super(ConvolutionalBackbone, self).__init__()
		self.img_dims = img_dims
		# basically using the number of dims and the number of poolings to be used 
		# figure out the size of the last fc layer so that this network is general to 
		# any images
		self.out_fc_dim = np.copy(img_dims)
		padvals = [4, 8, 8]
		for i in range(3):
			self.out_fc_dim[0] = net_utils.poolOutDim(self.out_fc_dim[0] - padvals[i], 2)
			self.out_fc_dim[1] = net_utils.poolOutDim(self.out_fc_dim[1] - padvals[i], 2)
			self.out_fc_dim[2] = net_utils.poolOutDim(self.out_fc_dim[2] - padvals[i], 2)

		if (self.out_fc_dim[0] < 1) or (self.out_fc_dim[1] < 1) or (self.out_fc_dim[2] < 1):
			raise Exception("Image dimensions are too small for the network.  Try reducing the image spacing.  ")

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

			('flatten', net_utils.Flatten()),
			
			('fc1', nn.Linear(self.out_fc_dim[0]*self.out_fc_dim[1]*self.out_fc_dim[2]*192, 384)),
			('relu6', nn.PReLU()),
			('fc2', nn.Linear(384,96)),
			('relu7', nn.PReLU()),
		]))
	def forward(self, x):
		x_features = self.features(x)
		return x_features

class DeterministicEncoder(nn.Module):
	def __init__(self, num_latent, img_dims, loader_dir):
		super(DeterministicEncoder, self).__init__()
		if torch.cuda.is_available():
			device = 'cuda:0'
		else:
			device = 'cpu'
		self.device = device
		self.num_latent = num_latent
		self.img_dims = img_dims
		self.loader_dir = loader_dir
		self.ConvolutionalBackbone = ConvolutionalBackbone(self.img_dims)
		self.pca_pred = nn.Sequential(OrderedDict([
			('linear', nn.Linear(96, self.num_latent))
		]))

	def forward(self, x):
		x = self.ConvolutionalBackbone(x)
		pca_load = self.pca_pred(x)
		pca_load_unwhiten = net_utils.unwhiten_PCA_scores(pca_load, self.loader_dir, self.device)
		return pca_load, pca_load_unwhiten

class DeterministicLinearDecoder(nn.Module):
	def __init__(self, num_latent, num_corr):
		super(DeterministicLinearDecoder, self).__init__()
		self.num_latent = num_latent
		self.numL = num_corr
		self.fc_fine = nn.Linear(self.num_latent, self.numL*3)

	def forward(self, pca_load_unwhiten):
		corr_out = self.fc_fine(pca_load_unwhiten).reshape(-1, self.numL, 3)
		return corr_out

'''
Supervised DeepSSM Model
'''
class DeepSSMNet(nn.Module):
	def __init__(self, config_file):
		super(DeepSSMNet, self).__init__()
		if torch.cuda.is_available():
			device = 'cuda:0'
		else:
			device = 'cpu'
		self.device = device
		with open(config_file) as json_file: 
			parameters = json.load(json_file)
		self.num_latent = parameters['num_latent_dim']
		self.loader_dir = parameters['paths']['loader_dir']
		loader = torch.load(self.loader_dir + "validation")
		self.num_corr = loader.dataset.mdl_target[0].shape[0]
		img_dims = loader.dataset.img[0].shape
		self.img_dims = img_dims[1:]
		# encoder
		if parameters['encoder']['deterministic']:
			self.encoder = DeterministicEncoder(self.num_latent, self.img_dims, self.loader_dir )
		if not self.encoder:
			print("Error: Encoder not implemented.")
		# decoder
		if parameters['decoder']['deterministic']:
			if parameters['decoder']['linear']:
				self.decoder = DeterministicLinearDecoder(self.num_latent, self.num_corr)
		if not self.decoder:
			print("Error: Decoder not implemented.")

	def forward(self, x):
		pca_load, pca_load_unwhiten = self.encoder(x)
		corr_out = self.decoder(pca_load_unwhiten)
		return [pca_load, corr_out]

class CorrespondenceEncoder(nn.Module):
	def __init__(self, num_latent, num_corr):
		super(CorrespondenceEncoder, self).__init__()
		self.num_latent = num_latent
		self.num_corr = num_corr
		self.dims = 3
		self.encoder = nn.Sequential(OrderedDict([
			('fc-en1', nn.Linear(self.dims*self.num_corr, 1024)),
			('relu-en1', nn.PReLU()),
			('fc-en2', nn.Linear(1024, 512)),
			('relu-en2', nn.PReLU()),
			('fc-en3', nn.Linear(512, self.num_latent))
		]))

	def forward(self, z):
		z_out = self.encoder(z)
		return z_out

class CorrespondenceDecoder(nn.Module):
	def __init__(self, num_latent, num_corr):
		super(CorrespondenceDecoder, self).__init__()
		self.num_latent = num_latent
		self.num_corr = num_corr
		self.dims = 3
		self.decoder = nn.Sequential(OrderedDict([
			('fc-de1', nn.Linear(self.num_latent, 512)),
			('relu-de1', nn.PReLU()),
			('fc-de2', nn.Linear(512, 1024)),
			('relu-de2', nn.PReLU()),
			('fc-de3', nn.Linear(1024, self.num_corr*self.dims))
		]))

	def forward(self, z):
		pt_out = self.decoder(z)
		return pt_out
	
"""
DeepSSM TL-Net Model
"""
class DeepSSMNet_TLNet(nn.Module):
	def __init__(self, conflict_file):
		super(DeepSSMNet_TLNet, self).__init__()
		if torch.cuda.is_available():
			device = 'cuda:0'
		else:
			device = 'cpu'
		self.device = device
		with open(conflict_file) as json_file: 
			parameters = json.load(json_file)
		self.num_latent = parameters['num_latent_dim']
		self.loader_dir = parameters['paths']['loader_dir']
		loader = torch.load(self.loader_dir + "validation")
		self.num_corr = loader.dataset.mdl_target[0].shape[0]
		img_dims = loader.dataset.img[0].shape
		self.img_dims = img_dims[1:]
		self.CorrespondenceEncoder = CorrespondenceEncoder(self.num_latent, self.num_corr)
		self.CorrespondenceDecoder = CorrespondenceDecoder(self.num_latent, self.num_corr)
		self.ImageEncoder = DeterministicEncoder(self.num_latent, self.img_dims, self.loader_dir)

	def forward(self, pt, x):
		# for testing
		if len(pt.shape) < 3:
			zt, _ = self.ImageEncoder(x)
			pt_out = self.CorrespondenceDecoder(zt)
			return [zt, pt_out.reshape(-1, self.num_corr, 3)]
		# for training
		else:
			pt1 = pt.view(-1, pt.shape[1]*pt.shape[2])
			z = self.CorrespondenceEncoder(pt1)
			pt_out = self.CorrespondenceDecoder(z)
			zt, _ = self.ImageEncoder(x)
		return [pt_out.view(-1, pt.shape[1], pt.shape[2]), z, zt]