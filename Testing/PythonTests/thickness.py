import os
import sys
from shapeworks import *

success = True

def thicknessTest():
  sphere = Mesh(os.environ["DATA"] + "/thickness/sphere.vtk")
  ct = Image(os.environ["DATA"] + "/thickness/ct.nrrd")
  thickness = sphere.computeThickness(ct)

  compareMesh = Mesh(os.environ["DATA"] + "/thickness/thickness.vtk")

  return thickness == compareMesh

success &= utils.test(thicknessTest)

sys.exit(not success)
