import os
import sys
import shapeworks as sw

def invertNormalsTest():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.invertNormals()

  compareMesh = sw.Mesh(os.environ["DATA"] + "/invertnormals.vtk")

  return mesh == compareMesh

val = invertNormalsTest()

if val is False:
  sys.exit(1)
