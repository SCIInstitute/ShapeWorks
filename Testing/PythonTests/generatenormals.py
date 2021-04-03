import os
import sys
from shapeworks import *

def generateNormalsTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.generateNormals()

  compareMesh = Mesh(os.environ["DATA"] + "/normals.vtk")

  return mesh == compareMesh

val = generateNormalsTest()

if val is False:
  sys.exit(1)
