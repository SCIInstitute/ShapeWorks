import os
import sys
from shapeworks import *

def computemeannormalsTest():
  mesh1 = Mesh(os.environ["DATA"] + "/m03.vtk")
  mesh2 = Mesh(os.environ["DATA"] + "/m04.vtk")

  meshList = []
  meshList.append(mesh1.generateNormals())
  meshList.append(mesh2.generateNormals())

  mesh = MeshUtils.computeMeanNormals(meshList)

  ground_truth = Mesh(os.environ["DATA"] + "/meannormals.vtk")

  return mesh.compareField(ground_truth, "MeanNormals")

try:
  computemeannormalsTest()
except ValueError:
  print("computemeannormalsTest failed")
  sys.exit(1)
