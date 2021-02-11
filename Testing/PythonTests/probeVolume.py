import os
import sys
import shapeworks as sw

def probeVolumeTest():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  img = sw.Image(os.environ["DATA"] + "/femurVtkDT.nrrd")
  mesh.probeVolume(img)

  compareMesh = sw.Mesh(os.environ["DATA"] + "/probe.vtk")

  return mesh == compareMesh

val = probeVolumeTest()

if val is False:
  sys.exit(1)