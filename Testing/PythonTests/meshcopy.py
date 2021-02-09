import os
import sys
from shapeworks import *

def copyTest1():
  mesh1 = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh2 = mesh1.copy()
  mesh1.translate([1, 10, -1])
  compareMesh1 = Mesh(os.environ["DATA"] + "/femurTranslated.vtk")
  compareMesh2 = Mesh(os.environ["DATA"] + "/femur.vtk")

  return mesh1 == compareMesh1 and mesh2 == compareMesh2;

val = copyTest1()

if val is False:
  sys.exit(1)

