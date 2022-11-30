import os
import sys
from shapeworks import *
import numpy as np

success = True

def geodesicTest1():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  p1 = 10
  p2 = 200
  geodesic_dist = mesh.geodesicDistance(p1, p2)

  pt1 = mesh.getPoint(p1)
  pt2 = mesh.getPoint(p2)
  euclidean_dist = np.linalg.norm(pt2 - pt1)

  return abs(geodesic_dist - 6.577) < 1e-4 and abs(euclidean_dist - 6.70625) < 1e-4

success &= utils.test(geodesicTest1)

def geodesicTest2():
  mesh = Mesh(os.environ["DATA"] + "/m03.vtk")
  distField = mesh.geodesicDistance(mesh.getPoint(200))
  mesh.setField("GeodesicDistanceToLandmark", distField, Mesh.Point)

  ground_truth = Mesh(os.environ["DATA"] + "/geodesic1.vtk")

  return mesh.compareField(ground_truth, "GeodesicDistanceToLandmark")

success &= utils.test(geodesicTest2)

def geodesicTest3():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")

  curve = []
  curve.append(mesh.getPoint(100))
  curve.append(mesh.getPoint(200))
  curve.append(mesh.getPoint(300))

  distField = mesh.geodesicDistance(curve)
  mesh.setField("GeodesicDistanceToCurve", distField, Mesh.Point)

  ground_truth = Mesh(os.environ["DATA"] + "/geodesic_curve_py.vtk")

  return mesh.compareField(ground_truth, "GeodesicDistanceToCurve")

success &= utils.test(geodesicTest3)

sys.exit(not success)
