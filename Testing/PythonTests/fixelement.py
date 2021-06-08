import os
import sys
from shapeworks import *

def fixelementTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.fixElement()

  compareMesh = Mesh(os.environ["DATA"] + "/fixelement.vtk")

  return mesh == compareMesh

val = fixelementTest()

if val is False:
  print("fixelementTest failed")
  sys.exit(1)
