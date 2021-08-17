import os
import sys
from shapeworks import *

def generatenormalsTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.generateNormals()

  compareMesh = Mesh(os.environ["DATA"] + "/normals.vtk")

  return mesh == compareMesh

utils.test(generatenormalsTest)
