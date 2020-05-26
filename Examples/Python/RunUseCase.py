# -*- coding: utf-8 -*-

"""
Most of the following steps even though wrapped in python functions are using
the underlying c++ code, for which we need to call the source paths to the
binaries. This step should be common for any use of a function in ShapeWorks.
__This requires the full ShapeWorks to be correctly built/downloaded!__'

These following commands set the temporary environment variables to point to
shapeworks binaries and set the necessary library paths
"""
import os
import platform
import argparse
import subprocess
import sys


# check that required modules are found
try:
    import termcolor
    import DatasetUtils
except ImportError as error:
    print("\nError: Unable to import ShapeWorks Python dependencies\n")
    print("Please make sure you have run \"source conda_installs.sh\" (once) and \"conda activate shapeworks\" (each time)")
    print("See https://github.com/SCIInstitute/ShapeWorks for more information")
    sys.exit(1)


# Path pre-setup
binpath = "../build/shapeworks/src/ShapeWorks-build/bin:../../bin"
if platform.system() == "Windows":
    binpath = "C:\\Program Files\ShapeWorks\\bin"
if platform.system() == "Darwin":
    binpath = binpath + ":/Applications/ShapeWorks/bin"

parser = argparse.ArgumentParser(description='Example ShapeWorks Pipeline')
parser.add_argument("--use_case", help="Specify which use case to run, either: ellipsoid, ellipsoid_fd, left_atrium, or femur.")
parser.add_argument("--use_subsample", help="Set number of samples to run the pipeline for a subset of data.")
parser.add_argument("--interactive", help="Run in interactive mode", action="store_true")
parser.add_argument("--start_with_prepped_data", help="Start with already prepped data", action="store_true")
parser.add_argument("--start_with_image_and_segmentation_data", help = "use images and segmentations data for preprocessing", action="store_true")
parser.add_argument("--use_single_scale", help="Single scale or multi scale optimization", action="store_true")
parser.add_argument("--tiny_test", help="Run as a short test", action="store_true")
parser.add_argument("shapeworks_path", help="Path to ShapeWorks executables (default: "+binpath+")", nargs='?', type=str, default=binpath)
args = parser.parse_args()
binpath = args.shapeworks_path

if len(sys.argv)==1:
    parser.print_help(sys.stderr)
    sys.exit(1)

# local path for imports, etc
sys.path.insert(0, binpath + "/ShapeWorksStudio.app/Contents/MacOS")
sys.path.insert(0, binpath)

# paths for subprocesses (or could pass as env to each one)
if platform.system() == "Darwin":
    items = binpath.split(os.pathsep)
    binpath = ""
    for item in items:
        binpath = binpath + os.pathsep + item \
            + os.pathsep + item + "/ShapeWorksStudio.app/Contents/MacOS"
os.environ["PATH"] = binpath + os.pathsep + os.environ["PATH"]

module = __import__(args.use_case)

try:
    module.Run_Pipeline(args)
    print("\nShapeworks Pipeline Complete!")
except KeyboardInterrupt:
    print("KeyboardInterrupt exception caught")
    sys.exit(1)
except subprocess.CalledProcessError as e:
    print("General exception caught.\n\tReturncode: "+str(e.returncode)+"\n\tOutput: "+str(e.output))
