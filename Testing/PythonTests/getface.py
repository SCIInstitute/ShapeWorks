import os
import numpy as np
from shapeworks import *

def getFaceTest1():
  mesh = Mesh(os.environ["DATA"] + "/simple_ellipsoid.ply")
  f = mesh.getFace(12)
  face = [9,12,1]

  return (f == face).all()

try:
  if not getFaceTest1():
    print("getFaceTest1 failed")
    sys.exit(1)
except RuntimeError:
  print("getFaceTest1 failed (exception)")
  sys.exit(1)
