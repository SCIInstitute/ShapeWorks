import os
import sys
from shapeworks import *
import numpy as np

def geodesicTest1():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  dist = mesh.geodesicDistance(10, 20)

  return abs(dist - 1.64581) < 1e-4

utils.test(geodesicTest1)

def geodesicTest2():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")
  distField = mesh.geodesicDistance(mesh.getPoint(50))
  mesh.setField("GeodesicDistanceToLandmark", distField)

  ground_truth = Mesh(os.environ["DATA"] + "/geodesic1.vtk")

  return mesh.compareField(ground_truth, "GeodesicDistanceToLandmark")

utils.test(geodesicTest2)

def geodesicTest3():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")

  curve = []
  curve.append(mesh.getPoint(100))
  curve.append(mesh.getPoint(200))
  curve.append(mesh.getPoint(300))

  distField = mesh.geodesicDistance(curve)
  mesh.setField("GeodesicDistanceToCurve", distField)

  ground_truth = Mesh(os.environ["DATA"] + "/geodesic2.vtk")

  return mesh.compareField(ground_truth, "GeodesicDistanceToCurve")

utils.test(geodesicTest3)
