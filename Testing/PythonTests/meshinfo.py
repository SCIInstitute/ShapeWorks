import os
import sys
from shapeworks import *

def infoTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  center = mesh.center()

  compareMesh = Mesh(os.environ["DATA"] + "/femur.ply")
  compareCenter = compareMesh.center()

  return center[0] == compareCenter[0] and center[1] == compareCenter[1] and center[2] == compareCenter[2]

val = infoTest3()

if val is False:
  sys.exit(1)

def infoTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  com = mesh.centerOfMass()

  compareMesh = Mesh(os.environ["DATA"] + "/femur.ply")
  compareCom = compareMesh.centerOfMass()

  return com[0] == compareCom[0] and com[1] == compareCom[1] and com[2] == compareCom[2]

val = infoTest4()

if val is False:
  sys.exit(1)
