import os
import sys
from shapeworks import *
import numpy as np

def geodesicTest1():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  dist = mesh.geodesicDistance(10, 20)

  return abs(dist - 1.0083) < 1e-4

utils.test(geodesicTest1)

// TODO: Need to finish this test
# def geodesicTest2():
#   mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
#   dist = mesh.geodesicDistance(mesh.getPoint(50))
  
#   ground_truth = np.loadtxt("landmark.txt")

#   return np.linalg.norm(dist-ground_truths) < 1e-4

# utils.test(geodesicTest2)
