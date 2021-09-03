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

  arr = mesh.curvature()
  mesh.setField(arr, "PrincipalCurvature")

  ground_truth = Mesh(os.environ["DATA"] + "/principalcurvature.vtk")

  return mesh.compareField(ground_truth, "PrincipalCurvature")

utils.test(curvatureTest2)

def curvatureTest3():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")

  arr = mesh.curvature(Mesh.CurvatureType.Gaussian)
  mesh.setField(arr, "GaussianCurvature")

  ground_truth = Mesh(os.environ["DATA"] + "/gaussiancurvature.vtk")

  return mesh.compareField(ground_truth, "GaussianCurvature")

utils.test(curvatureTest3)

def curvatureTest4():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")

  arr = mesh.curvature(Mesh.CurvatureType.Mea)

utils.expectException(curvatureTest4, AttributeError)
