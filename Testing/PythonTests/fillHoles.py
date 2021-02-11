import os
import sys
import shapeworks as sw

def fillholesTest():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.fillHoles()

  compareMesh = sw.Mesh(os.environ["DATA"] + "/fillholes.vtk")

  return mesh == compareMesh

val = fillholesTest()

if val is False:
  sys.exit(1)