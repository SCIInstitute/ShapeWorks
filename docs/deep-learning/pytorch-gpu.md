# PyTorch GPU Support for ShapeWorks

ShapeWorks deep learning tools (the `DeepSSMUtils` package and `shapeworks
deepssm` CLI) require PyTorch. ShapeWorks installs PyTorch on demand the first
time you use a DeepSSM feature.

## How the on-demand install works

The first time a DeepSSM operation runs — whether through ShapeWorks Studio,
the `shapeworks deepssm` CLI, or `swpython RunUseCase.py deep_ssm` — ShapeWorks
uses [`light-the-torch`](https://github.com/pmeier/light-the-torch) to detect
your CUDA driver and download the matching PyTorch wheel. If CUDA is supported,
the GPU build is installed; otherwise the CPU build is used.

PyTorch is installed into a per-user directory that persists across ShapeWorks
sessions (and survives reinstalls of the bundle):

  * **macOS:** `~/Library/Application Support/ShapeWorks/<version>/site-packages/`
  * **Linux:** `~/.local/share/ShapeWorks/<version>/site-packages/`
  * **Windows:** `%LOCALAPPDATA%\ShapeWorks\<version>\site-packages\`

The first install downloads ~3 GB; subsequent runs reuse the cached copy.

## Checking if PyTorch has GPU support

Run from the **ShapeWorks Prompt** (Windows) or a terminal with the ShapeWorks
`bin/` on `PATH` (macOS / Linux):

```
swpython -c "import torch; print(torch.cuda.is_available())"
```

If this prints `True`, PyTorch has GPU support. If it prints `False`, the CPU
build was installed (no compatible CUDA driver was detected). If `torch` cannot
be imported, no DeepSSM operation has run yet and PyTorch has not been
installed — kick one off and try again, or trigger the install manually:

```
swpython -c "from shapeworks import ensure_torch; ensure_torch()"
```

## "No kernel image is available for execution on the device"

PyTorch wheels only ship kernels for a fixed set of GPU architectures, so a card
that is newer or older than the installed wheel supports is detected by
`torch.cuda.is_available()` but fails at the first kernel launch. ShapeWorks
probes the GPU before using it and stops with a message naming your GPU's
compute capability and the architectures the wheel supports:

```
Your GPU (NVIDIA GeForce GTX 1080, compute capability 6.1) cannot run this PyTorch build:
  CUDA error: no kernel image is available for execution on the device
PyTorch 2.9.1 (CUDA 12.8) only has kernels for: sm_75, sm_80, sm_86, sm_90, sm_100, sm_120
```

Install a build that covers your card (see below): older cards (Maxwell, Pascal)
need an older PyTorch with a CUDA 12.6 or earlier wheel, while Blackwell cards
(RTX 50-series) need CUDA 12.8 or newer. DeepSSM does not fall back to the CPU
in this case — a machine with a GPU it cannot use is a setup problem worth
fixing, not a reason to start a run that would take days.

## Reinstalling a different PyTorch version

If you need a different PyTorch version than `light-the-torch` selected:

1. Uninstall the current PyTorch:
   ```
   swpip uninstall torch torchvision torchaudio
   ```
2. Check your CUDA version (see [CUDA compatibility](https://docs.nvidia.com/deploy/cuda-compatibility/)
   and [How to check CUDA version](https://varhowto.com/check-pytorch-cuda-version/)).
3. Install the version you want using `swpip` and the appropriate index URL from
   [PyTorch Getting Started](https://pytorch.org/get-started/locally/):
   ```
   swpip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu<VERSION>
   ```
   Where `<VERSION>` is your CUDA version with no dot (such as `121` for CUDA 12.1).
4. Verify with the GPU-support check above.
