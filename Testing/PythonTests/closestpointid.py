import os
import sys
from shapeworks import *

def closestPointIdTest():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  p = mesh.getPoint(50)
  id = mesh.closestPointId(p)

  return id == 50

try:
  if not closestPointIdTest():
    print("closestPointIdTest failed")
    sys.exit(1)
except RuntimeError:
  print("closestPointIdTest failed (exception)")
  sys.exit(1)