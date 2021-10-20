import os
import sys
from shapeworks import *

success = True

def coverageTest():
  femur = Mesh(os.environ["DATA"] + "/femur.vtk")
  pelvis = Mesh(os.environ["DATA"] + "/pelvis.vtk")
  pelvis.coverage(femur)

  compareMesh = Mesh(os.environ["DATA"] + "/fm_coverage.vtk")

  return pelvis == compareMesh

success &= utils.test(coverageTest)

sys.exit(not success)
