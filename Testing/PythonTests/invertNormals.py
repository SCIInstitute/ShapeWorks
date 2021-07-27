import os
import sys
from shapeworks import *

def invertnormalsTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.invertNormals()

  compareMesh = Mesh(os.environ["DATA"] + "/invertnormals.vtk")

  return mesh == compareMesh

try:
  invertnormalsTest()
except ValueError:
  print("invertnormalsTest failed")
  sys.exit(1)
