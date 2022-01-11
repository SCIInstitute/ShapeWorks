import os
import sys
from shapeworks import *

success = True

def invertnormalsTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.invertNormals()

  compareMesh = Mesh(os.environ["DATA"] + "/invertnormals.vtk")

  return mesh == compareMesh

success &= utils.test(invertnormalsTest)

sys.exit(not success)
