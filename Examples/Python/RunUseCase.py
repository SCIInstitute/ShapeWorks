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
from CommonUtils import robustifyShapeworksPaths

# check that required modules are found
try:
    import termcolor
    import DatasetUtils
except ImportError as error:
    print("\nError: Unable to import ShapeWorks Python dependencies\n")
    print("Please make sure you have run \"source conda_installs.sh\" (once) and \"conda activate shapeworks\" (each time)")
    print("See https://github.com/SCIInstitute/ShapeWorks for more information")
    sys.exit(1)

# Default installation path for each platform. If using your own build, specify --shapeworks_path
default_binpath = "../../bin"
if platform.system() == "Windows":
    default_binpath = "C:\\Program Files\\ShapeWorks\\bin"
if platform.system() == "Darwin":
    default_binpath = "/Applications/ShapeWorks/bin:/Applications/ShapeWorks/bin/ShapeWorksStudio.app/Contents/MacOS"
default_subsample = 3

parser = argparse.ArgumentParser(description='Example ShapeWorks Pipeline')
parser.add_argument("--use_case", help="Specify which use case to run",
                    choices=["ellipsoid", "ellipsoid_fd", "ellipsoid_mesh", "lumps", "left_atrium", "femur", "femur_mesh", "deep_ssm"])
parser.add_argument("--use_subsample", help="Run the pipeline for a subset of data",action="store_true")
parser.add_argument("--num_subsample", help="Size of subset to run on (default: "+str(default_subsample)+")", nargs='?', type=int, default=default_subsample)
parser.add_argument("--interactive", help="Run in interactive mode", action="store_true")
parser.add_argument("--start_with_prepped_data", help="Start with already prepped data", action="store_true")
parser.add_argument("--start_with_image_and_segmentation_data", help = "use images and segmentations data for preprocessing", action="store_true")
parser.add_argument("--use_single_scale", help="Single scale or multi scale optimization", action="store_true")
parser.add_argument("--tiny_test", help="Run as a short test", action="store_true")
parser.add_argument("--shapeworks_path", help="Path to ShapeWorks executables (default: "+default_binpath+")", nargs='?', type=str, default=os.pathsep)
args = parser.parse_args()
explicit_binpath = args.shapeworks_path

if args.use_subsample:
    args.use_subsample = args.num_subsample

if len(sys.argv)==1:
    parser.print_help(sys.stderr)
    sys.exit(1)

module = __import__(args.use_case.lower())

os.environ["PATH"] = explicit_binpath + os.pathsep + os.environ["PATH"] + os.pathsep + default_binpath

# make sure the shapeworks executables can be found
robustifyShapeworksPaths()

try:
    module.Run_Pipeline(args)
    print("\nShapeworks Pipeline Complete!")
except KeyboardInterrupt:
    print("KeyboardInterrupt exception caught")
    sys.exit(1)
except subprocess.CalledProcessError as e:
    print("General exception caught.\n\tReturncode: "+str(e.returncode)+"\n\tOutput: "+str(e.output))
