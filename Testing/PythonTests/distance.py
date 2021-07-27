import os
import sys
from shapeworks import *

def distanceTest1():
  femur = Mesh(os.environ["DATA"] + "/femur.vtk")
  pelvis = Mesh(os.environ["DATA"] + "/pelvis.vtk")
  femur.distance(pelvis)
  pelvis.distance(femur)

  f2p = Mesh(os.environ["DATA"] + "/meshdistance2.vtk")
  p2f = Mesh(os.environ["DATA"] + "/meshdistance2rev.vtk")

  return femur == f2p and pelvis == p2f

try:
  distanceTest1()
except ValueError:
  print("distanceTest1 failed")
  sys.exit(1)

def distanceTest2():
  femur1 = Mesh(os.environ["DATA"] + "/m03_L_femur.ply")
  femur2 = Mesh(os.environ["DATA"] + "/m04_L_femur.ply")
  femur1.distance(femur2, Mesh.DistanceMethod.POINT_TO_CELL)
  femur2.distance(femur1)

  fwd = Mesh(os.environ["DATA"] + "/meshdistance1p2c.vtk")
  rev = Mesh(os.environ["DATA"] + "/meshdistance1rev.vtk")

  return femur1 == fwd and femur2 == rev

try:
  distanceTest2()
except ValueError:
  print("distanceTest2 failed")
  sys.exit(1)
