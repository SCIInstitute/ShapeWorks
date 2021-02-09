import os
import sys
import shapeworks as sw

def coverageTest():
  femur = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  pelvis = sw.Mesh(os.environ["DATA"] + "/pelvis.vtk")
  pelvis.coverage(femur)

  compareMesh = sw.Mesh(os.environ["DATA"] + "/fm_coverage.vtk")

  return pelvis == compareMesh

val = coverageTest()

if val is False:
  sys.exit(1)