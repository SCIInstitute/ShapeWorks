import os
import sys
import shapeworks as sw

def distanceTest1():
  femur = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  pelvis = sw.Mesh(os.environ["DATA"] + "/pelvis.vtk")
  femur.distance(pelvis)
  pelvis.distance(femur)

  f2p = sw.Mesh(os.environ["DATA"] + "/meshdistance2.vtk")
  p2f = sw.Mesh(os.environ["DATA"] + "/meshdistance2rev.vtk")

  return femur == f2p and pelvis == p2f

val = distanceTest1()

if val is False:
  sys.exit(1)

def distanceTest2():
  femur1 = sw.Mesh(os.environ["DATA"] + "/m03_L_femur.ply")
  femur2 = sw.Mesh(os.environ["DATA"] + "/m04_L_femur.ply")
  femur1.distance(femur2, sw.DistanceMethod.POINT_TO_CELL)
  femur2.distance(femur1)

  fwd = sw.Mesh(os.environ["DATA"] + "/meshdistance1p2c.vtk")
  rev = sw.Mesh(os.environ["DATA"] + "/meshdistance1rev.vtk")

  return femur1 == fwd and femur2 == rev

val = distanceTest2()

if val is False:
  sys.exit(1)