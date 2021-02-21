import os
import sys
from shapeworks import *

def invertNormalsTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.invertNormals()

  compareMesh = Mesh(os.environ["DATA"] + "/invertnormals.vtk")

  return mesh == compareMesh

val = invertNormalsTest()

if val is False:
  sys.exit(1)
