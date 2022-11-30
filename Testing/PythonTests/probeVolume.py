import os
import sys
from shapeworks import *

success = True

def probevolumeTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  img = Image(os.environ["DATA"] + "/femurVtkDT.nrrd")
  mesh.probeVolume(img)

  compareMesh = Mesh(os.environ["DATA"] + "/probe.vtk")

  return mesh == compareMesh

success &= utils.test(probevolumeTest)

sys.exit(not success)
