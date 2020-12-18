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
def setup_shapeworks_env(shapeworks_bin_dir = None,   # path to the binary directory of shapeworks
                         dependencies_bin_dir = None, # only needed if shapeworks is built from source: path to the binary directory of shapeworks dependencies used when running build_dependencies.sh
                         verbose = True):

    # if not set, assume a binary deployment and guess at location
    if shapeworks_bin_dir is None:
        if platform.system() == "Windows":
            shapeworks_bin_dir = "C:\\Program Files\\ShapeWorks\\bin"
        elif platform.system() == "Darwin": # MacOS
            shapeworks_bin_dir = "/Applications/ShapeWorks/bin"
        else: # Linux
            shapeworks_bin_dir   = "../../../../bin"
        
    # add shapeworks (and studio on mac) directory to python path 
    sys.path.append(shapeworks_bin_dir)
    if platform.system() == "Darwin": # MacOS
        sys.path.append(shapeworks_bin_dir + "/ShapeWorksStudio.app/Contents/MacOS")
    
    # add shapeworks and studio to the system path
    os.environ["PATH"] = shapeworks_bin_dir   + os.pathsep + os.environ["PATH"]
    if dependencies_bin_dir is not None:
        os.environ["PATH"] = dependencies_bin_dir + os.pathsep + os.environ["PATH"]
    if platform.system() == "Darwin": # MacOS
        os.environ["PATH"] = shapeworks_bin_dir + "/ShapeWorksStudio.app/Contents/MacOS" + os.pathsep + os.environ["PATH"]
    
    if verbose:
        print_python_path()
        print_env_path()
        
