import random
import torch
from torch import nn
import numpy as np
from shapeworks.utils import sw_message
from DeepSSMUtils import constants as C

# cached result of get_device(), the GPU probe only needs to run once
_device = None


def get_device() -> str:
	"""
	Get the device DeepSSM should run on.

	A GPU can be visible to torch and still be unable to run it: PyTorch wheels
	only ship kernels for a fixed set of GPU architectures, so a card that is
	newer or older than the installed wheel supports fails at the first kernel
	launch with "no kernel image is available for execution on the device".
	Probe the GPU once with a small kernel and fall back to the CPU if it fails.

	Returns:
		'cuda:0' if a GPU is present and usable, 'cpu' otherwise
	"""
	global _device
	if _device is None:
		_device = C.DEVICE_CUDA if _probe_gpu() else C.DEVICE_CPU
	return _device


def gpu_available() -> bool:
	"""
	Check whether a GPU is present and able to run this PyTorch build.

	Returns:
		True if DeepSSM will run on the GPU
	"""
	return get_device() == C.DEVICE_CUDA


def empty_gpu_cache() -> None:
	"""Free cached GPU memory, a no-op when not running on the GPU."""
	if gpu_available():
		torch.cuda.empty_cache()


def _probe_gpu() -> bool:
	"""Run a small kernel on the GPU, warning and returning False if it cannot run."""
	if not torch.cuda.is_available():
		return False
	try:
		probe = torch.zeros(8, 8, device=C.DEVICE_CUDA)
		torch.matmul(probe, probe)
		torch.cuda.synchronize()
		return True
	except Exception as e:
		_warn_gpu_unusable(e)
		return False


def _warn_gpu_unusable(error: Exception) -> None:
	"""Explain why the GPU was rejected and how to install a PyTorch build that supports it."""
	try:
		name = torch.cuda.get_device_name(0)
		major, minor = torch.cuda.get_device_capability(0)
		capability = f"{major}.{minor}"
	except Exception:
		# the device is unusable enough that even querying it fails
		name = "unknown"
		capability = "unknown"
	sw_message("********************* WARNING ****************************")
	sw_message(f"Your GPU ({name}, compute capability {capability}) cannot run this PyTorch build:")
	sw_message(f"  {str(error).splitlines()[0]}")
	sw_message(f"PyTorch {torch.__version__} (CUDA {torch.version.cuda}) only has kernels for: "
	           f"{', '.join(torch.cuda.get_arch_list())}")
	sw_message("Falling back to the CPU.  This will be very slow.")
	sw_message("To use the GPU, install a PyTorch build that supports it, for example:")
	sw_message("  swpip install torch --index-url https://download.pytorch.org/whl/cu128")
	sw_message("**********************************************************")


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