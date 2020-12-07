import os
import sys
import platform

# helper function to print out python path
def print_python_path():
    syspath = sys.path.copy()
    print("\nPython path:")
    for curpath in syspath:
        if curpath != "":
            print(curpath)
        
# helper function to print out system path
def print_env_path():
    syspath = os.environ["PATH"].split(os.pathsep)
    print("\nSystem path:")
    for curpath in syspath:
        if curpath != "":
            print(curpath)
        
# helper function to add shapeworks bin directory to the path
def setup_shapeworks_env(shapeworks_bin_dir,   # path to the binary directory of shapeworks
                         dependencies_bin_dir, # path to the binary directory of shapeworks dependencies used when running build_dependencies.sh
                         verbose = True):
    
    # add shapeworks (and studio on mac) directory to python path 
    sys.path.append(shapeworks_bin_dir)
    if platform.system() == "Darwin": # MacOS
        sys.path.append(shapeworks_bin_dir + "/ShapeWorksStudio.app/Contents/MacOS")
    
    # add shapeworks and studio to the system path
    os.environ["PATH"] = shapeworks_bin_dir   + os.pathsep + os.environ["PATH"]
    os.environ["PATH"] = dependencies_bin_dir + os.pathsep + os.environ["PATH"]
    if platform.system() == "Darwin": # MacOS
        os.environ["PATH"] = shapeworks_bin_dir + "/ShapeWorksStudio.app/Contents/MacOS" + os.pathsep + os.environ["PATH"]
    
    if verbose:
        print_python_path()
        print_env_path()
        