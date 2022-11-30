import os
import sys
from shapeworks import *

success = True

def writeTest1():
  ellipsoid = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk");
  ellipsoid.write(os.environ["DATA"] + "/ellipsoidBinary.vtk", True);

  ellipsoidBinary = Mesh(os.environ["DATA"] + "/ellipsoidBinary.vtk");

  return ellipsoid == ellipsoidBinary

success &= utils.test(writeTest1)

def writeTest2():
  ellipsoid = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply");
  ellipsoid.write(os.environ["DATA"] + "/ellipsoidBinary.ply", True);

  ellipsoidBinary = Mesh(os.environ["DATA"] + "/ellipsoidBinary.ply");

  return ellipsoid == ellipsoidBinary

success &= utils.test(writeTest2)

sys.exit(not success)
