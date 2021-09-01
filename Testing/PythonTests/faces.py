import os
import numpy as np
from shapeworks import *

def facesTest():
  mesh = Mesh(os.environ["DATA"] + "/simple_ellipsoid.ply")
  faces = mesh.faces()
  f0 = faces[0]
  g0 = [7,8,4]
  fn = faces[faces.shape[0]-1]
  gn = [7,6,8]

  return faces.shape[0] == 24 and faces.shape[1] == 3 and (f0 == g0).all() and (fn == gn).all()

utils.test(facesTest)
