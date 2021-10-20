import os
import sys
from shapeworks import *

success = True

def computenormalsTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.computeNormals()

  compareMesh = Mesh(os.environ["DATA"] + "/normals.vtk")

  return mesh == compareMesh

success &= utils.test(computenormalsTest)

sys.exit(not success)
