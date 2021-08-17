import os
import sys
from shapeworks import *

def computemeannormalsTest():
  mesh1 = Mesh(os.environ["DATA"] + "/m03.vtk")
  mesh2 = Mesh(os.environ["DATA"] + "/m04.vtk")

  meshList = []
  meshList.append(mesh1.generateNormals())
  meshList.append(mesh2.generateNormals())

  arr = MeshUtils.computeMeanNormals(meshList)
  mesh1.setField(arr, "MeanNormals")

  ground_truth = Mesh(os.environ["DATA"] + "/meannormals.vtk")

  return mesh1.compareField(ground_truth, "MeanNormals")

utils.test(computemeannormalsTest)
