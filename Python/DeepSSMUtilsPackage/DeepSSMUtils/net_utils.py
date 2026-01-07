import random
import torch
from torch import nn
import numpy as np
from DeepSSMUtils import constants as C


def set_seed(seed: int = 42) -> None:
	"""
	Set random seeds for reproducibility across all random number generators.

	Args:
		seed: Integer seed value for random number generators
	"""
	random.seed(seed)
	np.random.seed(seed)
	torch.manual_seed(seed)
	if torch.cuda.is_available():
		torch.cuda.manual_seed(seed)
		torch.cuda.manual_seed_all(seed)
		torch.backends.cudnn.deterministic = True
		torch.backends.cudnn.benchmark = False


class Flatten(nn.Module):
	"""Flatten layer to reshape tensor for fully connected layers."""

	def forward(self, x: torch.Tensor) -> torch.Tensor:
		return x.view(x.size(0), -1)


def poolOutDim(
	inDim: int,
	kernel_size: int,
	padding: int = 0,
	stride: int = 0,
	dilation: int = 1
) -> int:
	"""
	Calculate output dimension after pooling operation.

	Args:
		inDim: Input dimension size
		kernel_size: Size of the pooling kernel
		padding: Padding applied to input
		stride: Stride of pooling (defaults to kernel_size if 0)
		dilation: Dilation factor

	Returns:
		Output dimension size after pooling
	"""
	if stride == 0:
		stride = kernel_size
	num = inDim + 2*padding - dilation*(kernel_size - 1) - 1
	outDim = int(np.floor(num/stride + 1))
	return outDim


def unwhiten_PCA_scores(
	torch_loading: torch.Tensor,
	loader_dir: str,
	device: str
) -> torch.Tensor:
	"""
	Unwhiten (denormalize) PCA scores using saved mean and std.

	Args:
		torch_loading: Whitened PCA scores tensor
		loader_dir: Directory containing mean_PCA.npy and std_PCA.npy
		device: Device to load tensors to ('cuda:0' or 'cpu')

	Returns:
		Unwhitened PCA scores tensor
	"""
	mean_score = torch.from_numpy(np.load(loader_dir + '/' + C.MEAN_PCA_FILE)).to(device).float()
	std_score = torch.from_numpy(np.load(loader_dir + '/' + C.STD_PCA_FILE)).to(device).float()
	mean_score = mean_score.unsqueeze(0).repeat(torch_loading.shape[0], 1)
	std_score = std_score.unsqueeze(0).repeat(torch_loading.shape[0], 1)
	pca_new = torch_loading*(std_score) + mean_score
	return pca_new