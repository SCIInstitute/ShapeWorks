import os
import sys
from shapeworks import *

success = True

def distanceTest1():
  femur = Mesh(os.environ["DATA"] + "/femur.vtk")
  pelvis = Mesh(os.environ["DATA"] + "/pelvis.vtk")
  femur.setField("distance", femur.vertexDistance(pelvis))
  pelvis.setField("distance", pelvis.vertexDistance(femur))

  f2p = Mesh(os.environ["DATA"] + "/meshdistance2.vtk")
  p2f = Mesh(os.environ["DATA"] + "/meshdistance2rev.vtk")

  return femur == f2p and pelvis == p2f

success &= utils.test(distanceTest1)

def distanceTest2():
  femur1 = Mesh(os.environ["DATA"] + "/m03_L_femur.ply")
  femur2 = Mesh(os.environ["DATA"] + "/m04_L_femur.ply")
  femur1.setField("distance", femur1.distance(femur2))
  femur2.setField("distance", femur2.vertexDistance(femur1))

  fwd = Mesh(os.environ["DATA"] + "/meshdistance1p2c.vtk")
  rev = Mesh(os.environ["DATA"] + "/meshdistance1rev.vtk")

  return femur1 == fwd and femur2 == rev

success &= utils.test(distanceTest2)

sys.exit(not success)
