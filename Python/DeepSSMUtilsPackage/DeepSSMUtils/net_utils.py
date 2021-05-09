import torch
from torch import nn
import numpy as np

class Flatten(nn.Module):
	def forward(self, x):
		return x.view(x.size(0), -1)

def poolOutDim(inDim, kernel_size, padding=0, stride=0, dilation=1):
	if stride == 0:
		stride = kernel_size
	num = inDim + 2*padding - dilation*(kernel_size - 1) - 1
	outDim = int(np.floor(num/stride + 1))
	return outDim

def unwhiten_PCA_scores(torch_loading, loader_dir, device):
	mean_score = torch.from_numpy(np.load(loader_dir + '/mean_PCA.npy')).to(device).float()
	std_score = torch.from_numpy(np.load(loader_dir + '/std_PCA.npy')).to(device).float()
	mean_score = mean_score.unsqueeze(0).repeat(torch_loading.shape[0], 1)
	std_score = std_score.unsqueeze(0).repeat(torch_loading.shape[0], 1)
	pca_new = torch_loading*(std_score) + mean_score
	return pca_new