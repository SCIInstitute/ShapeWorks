import os
import sys
from shapeworks import *

def probeVolumeTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  img = Image(os.environ["DATA"] + "/femurVtkDT.nrrd")
  mesh.probeVolume(img)

  compareMesh = Mesh(os.environ["DATA"] + "/probe.vtk")

  return mesh == compareMesh

val = probeVolumeTest()

if val is False:
  sys.exit(1)
