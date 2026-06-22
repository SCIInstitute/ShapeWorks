import os
import sys


def setup_binary_path():
    # On Windows + bundled deployment, register the bundle's bin/ as a DLL
    # search directory so that shapeworks_py.pyd can find its dependent DLLs
    # (Qt, VTK, etc.). Python 3.8+ ignores PATH for native extension module
    # dependencies and only searches directories registered via
    # os.add_dll_directory or the Python install dir.
    if sys.platform == "win32" and hasattr(os, "add_dll_directory"):
        # Bundled layout: sys.prefix is <INSTDIR>/lib/python, DLLs are in <INSTDIR>/bin
        candidate = os.path.join(os.path.dirname(os.path.dirname(sys.prefix)), "bin")
        # Only add if it looks like a ShapeWorks bin dir (avoids polluting
        # dev/conda invocations where sys.prefix points elsewhere).
        if os.path.exists(os.path.join(candidate, "python312.dll")):
            os.add_dll_directory(candidate)

    try:
        import shapeworks_py
        return  # bundled — already in site-packages
    except ImportError:
        pass
    sys.path.append("placeholder_string")
