import os
import sys
from shapeworks import *

def computenormalsTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.computeNormals()

  compareMesh = Mesh(os.environ["DATA"] + "/normals.vtk")

  return mesh == compareMesh

utils.test(computenormalsTest)
