import os
import numpy as np
from shapeworks import *

def getPointTest1():
  mesh = Mesh(os.environ["DATA"] + "/simple_ellipsoid.ply")
  p = mesh.getPoint(7)
  closeToP = [44.7543, 2.43769, 12.953]

  return np.linalg.norm(p-closeToP) < 1e-4

try:
  if not getPointTest1():
    print("getPointTest1 failed")
    sys.exit(1)
except RuntimeError:
  print("getPointTest1 failed (exception)")
  sys.exit(1)
