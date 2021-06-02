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
import setupenv
import shutil
import shapeworks as sw

#check if current directory is writable
current_dir = os.getcwd()
if not os.access(current_dir,os.W_OK):
	raise OSError("You don't have write permission in the current directory. Please copy the Examples folder to a different location to run the use cases")


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
# Note, the path for linux differs from setupenv.py because it is set up for the notebooks
# that are two more levels deep
default_bin_path = "../../bin" # Linux
if platform.system() == "Windows":
    default_bin_path = "C:\\Program Files\\ShapeWorks\\bin" # Windows
if platform.system() == "Darwin":
    default_bin_path = "/Applications/ShapeWorks/bin" # Mac
default_subsample = 3

parser = argparse.ArgumentParser(description='Example ShapeWorks Pipeline')
parser.add_argument("--use_case", help="Specify which use case to run", choices=["ellipsoid", "ellipsoid_evaluate", "ellipsoid_mesh", "ellipsoid_fd", "ellipsoid_cut", "ellipsoid_multiple_domain","ellipsoid_multiple_domain_segs","lumps", "left_atrium", "femur", "femur_mesh", "femur_cut", "deep_ssm", "supershapes_1mode_contour", "thin_cavity_bean"])
parser.add_argument("--use_subsample", help="Run the pipeline for a subset of data",action="store_true")
parser.add_argument("--num_subsample", help="Size of subset to run on (default: "+str(default_subsample)+")", nargs='?', type=int, default=default_subsample)
parser.add_argument("--skip_grooming", help="Skip the grooming steps and start with already prepped (i.e., groomed) data", action="store_true")
parser.add_argument("--groom_images", help = "Apply grooming steps to both the shapes (segmentations or surface meshes) and raw images", action="store_true")
parser.add_argument("--use_single_scale", help="Use single scale optimization (default: multi scale)", action="store_true")
parser.add_argument("--tiny_test", help="Run as a short test", action="store_true")
parser.add_argument("--shapeworks_path", help="Path to ShapeWorks executables (default: "+default_bin_path+" if `shapeworks` is not already in the $PATH)", nargs='?', type=str, default=None)
args = parser.parse_args()
shapeworks_bin_path = args.shapeworks_path

#######################################
# Finding shapeworks, search priority:
#
# 1. if --shapeworks_path is given, use this
# 2. If `shapeworks` is in the current $PATH, use this
# 3. Platform dependent defaults
#######################################
if shapeworks_bin_path is None:
    swpath = shutil.which("shapeworks")
    if swpath:
        shapeworks_bin_path = os.path.dirname(swpath)      
if shapeworks_bin_path is None:
    shapeworks_bin_path = default_bin_path
setupenv.setup_shapeworks_env(shapeworks_bin_dir=shapeworks_bin_path, verbose=False)

# make sure the shapeworks executables can be found
swpath = shutil.which("shapeworks")
if (not swpath):
    print("Error: cannot find ShapeWorks executables. Please pass their location using the --shapeworks_path argument")
    sys.exit(1)

print(f"Using shapeworks from {shapeworks_bin_path}")

if args.use_subsample:
    dataExists = sw.data.dataset_exists_check(args.use_case)
    args.use_subsample = args.num_subsample
    if(dataExists==False):
        print("Please note : For --use_subsample argument the entire dataset will be downloaded.For a quick test use the --tiny_test argument")
        input("Press any key to continue")

if len(sys.argv)==1:
    parser.print_help(sys.stderr)
    sys.exit(1)

module = __import__(args.use_case.lower())

image_use_cases = ['femur', 'femur_cut', 'left_atrium']
if args.groom_images and args.use_case.lower() not in image_use_cases:
    print("\n\n*************************** WARNING ***************************")
    print("'groom_images' tag was used but use case does not have images.")
    print("Running use case with segmentations or meshes only.")
    print("***************************************************************\n\n")

try:
    module.Run_Pipeline(args)
    print("\nShapeworks Pipeline Complete!")
except KeyboardInterrupt:
    print("KeyboardInterrupt exception caught")
    sys.exit(1)
except subprocess.CalledProcessError as e:
    print("General exception caught.\n\tReturncode: "+str(e.returncode)+"\n\tOutput: "+str(e.output))
