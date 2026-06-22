"""On-demand PyTorch installation for DeepSSM workflows.

PyTorch is large and only needed for DeepSSM, so the bundled ShapeWorks
distribution does not include it. The C++ PythonWorker installs torch into a
versioned per-user site-packages directory on first use; this module performs
the same install from Python so that scripts invoked via `swpython` (Studio's
embedded interpreter is not in the call stack) get the same behavior.
"""

import importlib
import os
import platform
import subprocess
import sys

# Must match PythonWorker::python_api_version in Libs/Application/Job/PythonWorker.h.
# Bump alongside SW_MAJOR_VERSION / SW_MINOR_VERSION per docs/dev/release-checklist.
_API_VERSION = "6.8"


def _user_site_packages():
    home = os.path.expanduser("~")
    system = platform.system()
    if system == "Darwin":
        base = os.path.join(home, "Library", "Application Support", "ShapeWorks")
    elif system == "Windows":
        base = os.path.join(os.environ.get("LOCALAPPDATA", home), "ShapeWorks")
    else:
        base = os.path.join(home, ".local", "share", "ShapeWorks")
    return os.path.join(base, _API_VERSION, "site-packages")


def _add_user_site_to_path():
    user_site = _user_site_packages()
    if os.path.isdir(user_site) and user_site not in sys.path:
        sys.path.insert(0, user_site)


def is_torch_available():
    """Return True if torch can be imported (after adding the user-site path)."""
    _add_user_site_to_path()
    try:
        import torch  # noqa: F401
        return True
    except ImportError:
        return False


def ensure_torch():
    """Install PyTorch on demand if it isn't already importable.

    Installs into the same versioned user site-packages directory that
    ShapeWorks Studio and `shapeworks deepssm` use, via light-the-torch
    (which selects the correct CUDA/CPU wheel for the host).
    """
    if is_torch_available():
        return

    user_site = _user_site_packages()
    os.makedirs(user_site, exist_ok=True)
    print("PyTorch is not installed. Installing via light-the-torch...")
    print(f"Installing PyTorch to {user_site}")
    print("This may take several minutes...")

    subprocess.check_call([
        sys.executable, "-m", "light_the_torch", "install",
        "--target", user_site, "torch",
    ])

    importlib.invalidate_caches()
    if user_site not in sys.path:
        sys.path.insert(0, user_site)

    try:
        import torch  # noqa: F401
        print("PyTorch installed successfully.")
    except ImportError as e:
        raise RuntimeError(
            f"PyTorch install completed but cannot be imported: {e}"
        ) from e
