import os
import numpy as np
from shapeworks import *

def pointsTest1():
  mesh = Mesh(os.environ["DATA"] + "/simple_ellipsoid.ply")
  points = mesh.points()
  v0 = points[0]
  g0 = [1.12801208e+01,  1.84252377e+01,  2.66504917e+01]
  vn = points[points.shape[0]-1]
  gn = [3.35370102e+01,  1.25301433e+00,  3.71165695e+01]

  return points.shape[0] == 14 and points.shape[1] == 3 and np.linalg.norm(v0-g0) < 1e-4 and np.linalg.norm(vn-gn) < 1e-4

try:
  if not pointsTest1():
    print("pointsTest1 failed")
    sys.exit(1)
except RuntimeError:
  print("pointsTest1 failed (exception)")
  sys.exit(1)
