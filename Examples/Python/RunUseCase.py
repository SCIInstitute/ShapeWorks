# -*- coding: utf-8 -*-

"""
Most of the following steps even though wrapped in python functions are using
the underlying c++ code, for which we need to call the source paths to the
binaries. This step should be common for any use of a function in ShapeWorks.
__This requires the full ShapeWorks to be correctly built/downloaded!__'

For users: Please make sure you have run "source install_shapeworks.sh" (once) and "conda activate shapeworks" (each time).

For developers: Please make sure you have run "source ./devenv.sh <SOURCE_DIR> <BUILD_DIR>"
"""

import os
import platform
import argparse
import subprocess
import sys
import shutil

# check if current directory is writable
current_dir = os.getcwd()
if not os.access(current_dir, os.W_OK):
    raise OSError(
        "You don't have write permission in the current directory. Please copy the Examples folder to a different location to run the use cases")

if __name__ == '__main__':

    # check that required modules are found
    try:
        import termcolor
        import DatasetUtils
        import shapeworks as sw

        print(f"Using shapeworks module from {sw.__file__}")

        # make sure the shapeworks executables can be found
        swpath = shutil.which("shapeworks")
        print(f"Using shapeworks from {swpath}")
        if (not swpath):
            raise ImportError("Error: cannot find ShapeWorks executables.")

    except ImportError as error:
        print("\nError: Unable to import ShapeWorks executables, Python module, or dependencies\n")
        print(
            "Users: Please make sure you have run \"source install_shapeworks.sh\" (once) and \"conda activate shapeworks\" (each time)")
        print("\tSee http://sciinstitute.github.io/ShapeWorks/users/install.html for more information")
        print("\nDevelopers: Please make sure you have run \"source ./devenv.sh <SOURCE_DIR> <BUILD_DIR>\"")

        print(f"\nError message: {error}")
        sys.exit(1)

    # parse arguments
    parser = argparse.ArgumentParser(description='Example ShapeWorks Pipeline')
    parser.add_argument("use_case", help="Must specify which of these use cases to run.",
                        choices=["ellipsoid", "ellipsoid_evaluate", "ellipsoid_mesh", "ellipsoid_fd", "ellipsoid_cut",
                                 "ellipsoid_pca", \
                                 "ellipsoid_multiple_domain", "ellipsoid_multiple_domain_mesh", "lumps", "left_atrium", \
                                 "femur_cut", "femur_pvalues", "deep_ssm", "supershapes_1mode_contour",
                                 "thin_cavity_bean",
                                 "peanut_shared_boundary", "incremental_supershapes", "hip_multiple_domain"])
    parser.add_argument("--use_subsample", help="Run the pipeline for a subset of data", action="store_true")
    parser.add_argument("--num_subsample", help="Size of subset to run on (default: %(default)s)", type=int, default=3)
    parser.add_argument("--interactive", help="Run in interactive mode", action="store_true")
    parser.add_argument("--use_single_scale", help="Use single scale optimization (default: multi scale)",
                        action="store_true")
    parser.add_argument("--mesh_mode", help="Run optimization on meshes rather than distance transforms.",
                        action="store_true")
    parser.add_argument("--tiny_test", help="Run as a short test", action="store_true")
    parser.add_argument("--verify", help="Run as a full test", action="store_true")
    parser.add_argument("--clean", help="Run from scratch, ignoring intermediate stages", action="store_true")
    args = parser.parse_args()

    type = ""
    if args.tiny_test:
        type = "tiny_test_"
    scale = "multiscale"
    if args.use_single_scale:
        scale = "singlescale"
    mode = ""
    if args.mesh_mode:
        mode = "_mesh_mode"
    args.option_set = f"{type}{scale}{mode}"

    if args.use_subsample:
        dataExists = sw.data.dataset_exists_check(args.use_case)
        args.use_subsample = args.num_subsample
        if (dataExists == False):
            print(
                "Please note: For --use_subsample argument the entire dataset will be downloaded. For a quick test use the --tiny_test argument")
            input("Press any key to continue")

    # import use case and run
    module = __import__(args.use_case.lower())

    try:
        module.Run_Pipeline(args)
        print("\nShapeworks Pipeline Complete!")
    except KeyboardInterrupt:
        print("KeyboardInterrupt exception caught")
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        print(f"\nCalled Process Error:\n\tReturncode: {e.returncode}\n\tstdout: {e.stdout}\n\tstderr: {e.stderr}")
        sys.exit(1)
