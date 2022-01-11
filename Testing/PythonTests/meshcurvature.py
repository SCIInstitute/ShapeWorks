import os
import sys
from shapeworks import *

success = True

def meanCurvatureEllipsoidTest():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")

  arr = mesh.curvature(Mesh.CurvatureType.Mean)
  mesh.setField("MeanCurvature", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/meanCurvatureEllipsoid.vtk")

  return mesh.compareField(ground_truth, "MeanCurvature", eps=1e-14)

success &= utils.test(meanCurvatureEllipsoidTest)

def principalCurvatureEllipsoidTest():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")

  arr = mesh.curvature()
  mesh.setField("PrincipalCurvature", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/principalCurvatureEllipsoid.vtk")

  return mesh.compareField(ground_truth, "PrincipalCurvature", eps=1e-14)

success &= utils.test(principalCurvatureEllipsoidTest)

def gaussianCurvatureEllipsoidTest():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")

  arr = mesh.curvature(Mesh.CurvatureType.Gaussian)
  mesh.setField("GaussianCurvature", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/gaussianCurvatureEllipsoid.vtk")

  return mesh.compareField(ground_truth, "GaussianCurvature", eps=1e-14)

success &= utils.test(gaussianCurvatureEllipsoidTest)

def meanCurvatureFemurTest():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")

  arr = mesh.curvature(Mesh.CurvatureType.Mean)
  mesh.setField("MeanCurvature", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/meanCurvatureFemur.vtk")

  return mesh.compareField(ground_truth, "MeanCurvature", eps=1e-14)

success &= utils.test(meanCurvatureFemurTest)

def principalCurvatureFemurTest():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")

  arr = mesh.curvature()
  mesh.setField("PrincipalCurvature", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/principalCurvatureFemur.vtk")

  return mesh.compareField(ground_truth, "PrincipalCurvature", eps=1e-14)

success &= utils.test(principalCurvatureFemurTest)

def gaussianCurvatureFemurTest():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")

  arr = mesh.curvature(Mesh.CurvatureType.Gaussian)
  mesh.setField("GaussianCurvature", arr)

  ground_truth = Mesh(os.environ["DATA"] + "/gaussianCurvatureFemur.vtk")

  return mesh.compareField(ground_truth, "GaussianCurvature", eps=1e-14)

success &= utils.test(gaussianCurvatureFemurTest)

def curvaturefailTest():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")

  arr = mesh.curvature(Mesh.CurvatureType.Mea)

success &= utils.expectException(curvaturefailTest, AttributeError)

sys.exit(not success)
