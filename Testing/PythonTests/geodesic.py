import os
import sys
from shapeworks import *

def geodesicTest():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  dist = mesh.geodesicDistance(10, 20)

  return abs(dist - 1.0083) < 1e-4

utils.test(geodesicTest)
