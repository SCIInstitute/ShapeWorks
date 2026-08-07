import random
import torch
from torch import nn
import numpy as np
from DeepSSMUtils import constants as C

# cached results of get_device(), the GPU probe only needs to run once
_device = None
_device_error = None


class UnusableGPUError(RuntimeError):
	"""Raised when a GPU is present but cannot run the installed PyTorch build."""


def get_device() -> str:
	"""
	Get the device DeepSSM should run on.

	A GPU can be visible to torch and still be unable to run it: PyTorch wheels
	only ship kernels for a fixed set of GPU architectures, so a card that is
	newer or older than the installed wheel supports fails at the first kernel
	launch with "no kernel image is available for execution on the device".
	Probe the GPU once with a small kernel so that case is reported up front
	rather than crashing partway through a run.

	Returns:
		'cuda:0' if a GPU is present and usable, 'cpu' if there is no GPU at all

	Raises:
		UnusableGPUError: if a GPU is present but cannot run this PyTorch build
	"""
	global _device, _device_error
	if _device_error is not None:
		raise UnusableGPUError(_device_error)
	if _device is None:
		if not torch.cuda.is_available():
			_device = C.DEVICE_CPU
		else:
			try:
				probe = torch.zeros(8, 8, device=C.DEVICE_CUDA)
				torch.matmul(probe, probe)
				torch.cuda.synchronize()
			except Exception as e:
				_device_error = _unusable_gpu_message(e)
				raise UnusableGPUError(_device_error) from e
			_device = C.DEVICE_CUDA
	return _device


def gpu_available() -> bool:
	"""
	Check whether DeepSSM will run on the GPU.

	Returns:
		True if a GPU is present and usable, False if there is no GPU at all

	Raises:
		UnusableGPUError: if a GPU is present but cannot run this PyTorch build
	"""
	return get_device() == C.DEVICE_CUDA


def empty_gpu_cache() -> None:
	"""Free cached GPU memory, a no-op when not running on the GPU."""
	if gpu_available():
		torch.cuda.empty_cache()


def _unusable_gpu_message(error: Exception) -> str:
	"""Explain why the GPU was rejected and how to install a PyTorch build that supports it."""
	try:
		name = torch.cuda.get_device_name(0)
		major, minor = torch.cuda.get_device_capability(0)
		capability = f"{major}.{minor}"
	except Exception:
		# the device is unusable enough that even querying it fails
		name = "unknown"
		capability = "unknown"
	return "\n".join([
		f"Your GPU ({name}, compute capability {capability}) cannot run this PyTorch build:",
		f"  {str(error).splitlines()[0]}",
		f"PyTorch {torch.__version__} (CUDA {torch.version.cuda}) only has kernels for: "
		f"{', '.join(torch.cuda.get_arch_list())}",
		"Install a PyTorch build that supports your GPU, for example:",
		"  swpip install torch --index-url https://download.pytorch.org/whl/cu128",
		"See http://sciinstitute.github.io/ShapeWorks/latest/deep-learning/pytorch-gpu.html",
	])


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