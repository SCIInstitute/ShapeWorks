import os
import sys
import platform

def setup():
    build_dir = "../../build/bin"

    sys.path.insert(0, build_dir + "/ShapeWorksStudio.app/Contents/MacOS")
    sys.path.insert(0, build_dir)

    # paths for subprocesses (or could pass as env to each one)
    if platform.system() == "Darwin":
        items = build_dir.split(os.pathsep)
        build_dir = ""
        for item in items:
            build_dir = build_dir + os.pathsep + item \
                + os.pathsep + item + "/ShapeWorksStudio.app/Contents/MacOS"
    os.environ["PATH"] = build_dir + os.pathsep + os.environ["PATH"]

    test_dir = "../data/shapeworks/"
    os.environ["DATA"] = test_dir
