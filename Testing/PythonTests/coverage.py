import os
import sys
from shapeworks import *

def coverageTest():
  femur = Mesh(os.environ["DATA"] + "/femur.vtk")
  pelvis = Mesh(os.environ["DATA"] + "/pelvis.vtk")
  pelvis.coverage(femur)

  compareMesh = Mesh(os.environ["DATA"] + "/fm_coverage.vtk")

  return pelvis == compareMesh

try:
  coverageTest()
except ValueError:
  print("coverageTest failed")
  sys.exit(1)
