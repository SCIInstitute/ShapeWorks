import os
import sys
from shapeworks import *

def curvatureTest1():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")

  arr = mesh.curvature(Mesh.CurvatureType.Mean)
  mesh.setField(arr, "MeanCurvature")

  ground_truth = Mesh(os.environ["DATA"] + "/meancurvature.vtk")

  return mesh.compareField(ground_truth, "MeanCurvature")

utils.test(curvatureTest1)

def curvatureTest2():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")

  arr = mesh.curvature(Mesh.CurvatureType.Mea)
  mesh.setField(arr, "MeanCurvature")

  ground_truth = Mesh(os.environ["DATA"] + "/meancurvature.vtk")

  return mesh.compareField(ground_truth, "MeanCurvature")

utils.expectException(curvatureTest2, AttributeError)
