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
