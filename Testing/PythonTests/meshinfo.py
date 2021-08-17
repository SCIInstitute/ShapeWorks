import os
import sys
from shapeworks import *

def infoTest1():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  center = mesh.center()

  compareMesh = Mesh(os.environ["DATA"] + "/femur.ply")
  compareCenter = compareMesh.center()

  return center[0] == compareCenter[0] and center[1] == compareCenter[1] and center[2] == compareCenter[2]

utils.test(infoTest1)

def infoTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  com = mesh.centerOfMass()

  compareMesh = Mesh(os.environ["DATA"] + "/femur.ply")
  compareCom = compareMesh.centerOfMass()

  return com[0] == compareCom[0] and com[1] == compareCom[1] and com[2] == compareCom[2]

utils.test(infoTest2)
