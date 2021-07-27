import os
import sys
from shapeworks import *

def probevolumeTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  img = Image(os.environ["DATA"] + "/femurVtkDT.nrrd")
  mesh.probeVolume(img)

  compareMesh = Mesh(os.environ["DATA"] + "/probe.vtk")

  return mesh == compareMesh

try:
  probevolumeTest()
except ValueError:
  print("probevolumeTest failed")
  sys.exit(1)
