"""Live intensity augmentation for DeepSSM training.

Operates on the already normalized (z-scored) training images, on the GPU, without geometric resampling:
per sample, an optional Gaussian blur and gamma curve, then a random contrast scale, brightness shift and
Gaussian noise.
"""
import math
import torch
import torch.nn.functional as F

# scale/shift/noise are maximum magnitudes; gamma is the max |log(gamma)|; blur is the max sigma in voxels
DEFAULTS = {"scale": 0.1, "shift": 0.1, "noise": 0.05, "gamma": 0.0, "blur": 0.0}


def is_enabled(parameters):
    return parameters.get("intensity_augmentation", {}).get("enabled", False)


def gaussian_blur(img, sigma):
    """ Blur a (1, D, H, W) volume with an isotropic Gaussian (sigma in voxels), replicate padded """
    radius = max(1, math.ceil(2 * sigma))
    x = torch.arange(-radius, radius + 1, device=img.device, dtype=img.dtype)
    k = torch.exp(-x ** 2 / (2 * sigma ** 2))
    k = k / k.sum()
    out = F.pad(img[None], [radius] * 6, mode="replicate")
    for shape in ((1, 1, -1, 1, 1), (1, 1, 1, -1, 1), (1, 1, 1, 1, -1)):
        out = F.conv3d(out, k.reshape(shape))
    return out[0]


class IntensityAugmenter:
    def __init__(self, parameters):
        p = {**DEFAULTS, **parameters.get("intensity_augmentation", {})}
        self.scale, self.shift, self.noise = p["scale"], p["shift"], p["noise"]
        self.gamma, self.blur = p["gamma"], p["blur"]

    def __call__(self, img):
        n = img.shape[0]
        if self.blur > 0 or self.gamma > 0:
            samples = []
            for i in range(n):
                x = img[i]
                sigma = float(torch.rand(1)) * self.blur
                if sigma > 0.25:
                    x = gaussian_blur(x, sigma)
                if self.gamma > 0:
                    gamma = math.exp((float(torch.rand(1)) * 2 - 1) * self.gamma)
                    lo, hi = x.min(), x.max()
                    x = lo + (hi - lo) * ((x - lo) / (hi - lo + 1e-8)).clamp(0, 1) ** gamma
                samples.append(x)
            img = torch.stack(samples)
        shape = (n,) + (1,) * (img.dim() - 1)
        scale = 1 + (torch.rand(shape, device=img.device) * 2 - 1) * self.scale
        shift = (torch.rand(shape, device=img.device) * 2 - 1) * self.shift
        noise_std = torch.rand(shape, device=img.device) * self.noise
        return img * scale + shift + torch.randn_like(img) * noise_std
