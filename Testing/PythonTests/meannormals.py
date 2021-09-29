import os
import sys
from shapeworks import *

def computemeannormalsTest1():
  mesh1 = Mesh(os.environ["DATA"] + "/m03.vtk")
  mesh2 = Mesh(os.environ["DATA"] + "/m04.vtk")

  meshList = []
  meshList.append(mesh1.computeNormals())
  meshList.append(mesh2.computeNormals())

  arr = MeshUtils.computeMeanNormals(meshList)
  mesh1.setField("MeanNormals", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/meannormals.vtk")

  return mesh1.compareField(ground_truth, "MeanNormals")

utils.test(computemeannormalsTest1)

def computemeannormalsTest2():
  filenames = [os.environ["DATA"] + "/m03.vtk",
               os.environ["DATA"] + "/m04.vtk"]

  arr = MeshUtils.computeMeanNormals(filenames)

  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")
  mesh.setField("MeanNormals", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/meannormals.vtk")

  return mesh.compareField(ground_truth, "MeanNormals")

utils.test(computemeannormalsTest2)

def computemeannormalsTest3():
  filenames = [os.environ["DATA"] + "/m03.vtk",
               os.environ["DATA"] + "/m04.vtk"]

  arr = MeshUtils.computeMeanNormals(filenames, False)

  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")
  mesh.setField("MeanNormals", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/meannormals.vtk")

  return mesh.compareField(ground_truth, "MeanNormals")

utils.expectException(computemeannormalsTest3, ValueError)

def computemeannormalsTest4():
  mesh1 = Mesh(os.environ["DATA"] + "/m03.vtk")
  mesh2 = Mesh(os.environ["DATA"] + "/m04.vtk")

  meshList = []
  meshList.append(mesh1)
  meshList.append(mesh2)

  arr = MeshUtils.computeMeanNormals(meshList)
  mesh1.setField("MeanNormals", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/meannormals.vtk")

  return mesh1.compareField(ground_truth, "MeanNormals")

utils.expectException(computemeannormalsTest4, ValueError)
