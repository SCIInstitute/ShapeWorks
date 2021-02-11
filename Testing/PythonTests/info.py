import os
import sys
import shapeworks as sw

def infoTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  size1 = img.size()

  compareImg = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  size2 = compareImg.size()

  return size1[0] == size2[0] and size1[1] == size2[1] and size1[2] == size2[2]

val = infoTest1()

if val is False:
  sys.exit(1)

def infoTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  dims1 = img.dims()

  compareImg = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  dims2 = compareImg.dims()

  return dims1[0] == dims2[0] and dims1[1] == dims2[1] and dims1[2] == dims2[2]

val = infoTest2()

if val is False:
  sys.exit(1)

def infoTest3():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.ply")
  center = mesh.center()

  compareMesh = sw.Mesh(os.environ["DATA"] + "/femur.ply")
  compareCenter = compareMesh.center()

  return center[0] == compareCenter[0] and center[1] == compareCenter[1] and center[2] == compareCenter[2]

val = infoTest3()

if val is False:
  sys.exit(1)

def infoTest4():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.ply")
  com = mesh.centerOfMass()

  compareMesh = sw.Mesh(os.environ["DATA"] + "/femur.ply")
  compareCom = compareMesh.centerOfMass()

  return com[0] == compareCom[0] and com[1] == compareCom[1] and com[2] == compareCom[2]

val = infoTest4()

if val is False:
  sys.exit(1)