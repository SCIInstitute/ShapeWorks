import os
import numpy as np
from shapeworks import *

def getfaceTest():
  mesh = Mesh(os.environ["DATA"] + "/simple_ellipsoid.ply")
  f = mesh.getFace(12)
  face = [9,12,1]

  return (f == face).all()

try:
  getfaceTest()
except ValueError:
  print("getfaceTest failed")
  sys.exit(1)
