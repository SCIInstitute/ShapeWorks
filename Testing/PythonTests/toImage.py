import os
import sys
from shapeworks import *

def toImageTest1():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  img = mesh.toImage()

  compareImg = Image(os.environ["DATA"] + "/femurImage.nrrd")

  return img == compareImg

val = toImageTest1()

if val is False:
  print("toImageTest1 failed")
  sys.exit(1)

def toImageTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  bbox = mesh.boundingBox()
  bbox.pad(1)
  bbox.max = [bbox.max[0] / 2, bbox.max[1] / 2, bbox.max[2]]
  img = mesh.toImage(spacing=[2.0, 2.0, 1.0])

  compareImg = Image(os.environ["DATA"] + "/femurImage2.nrrd")

  return img == compareImg

val = toImageTest2()

if val is False:
  print("toImageTest2 failed")
  sys.exit(1)

def toImageTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  img = mesh.toImage(dims=[40, 145, 131])

  compareImg = Image(os.environ["DATA"] + "/femurImage3.nrrd")

  return img == compareImg

val = toImageTest3()

if val is False:
  print("toImageTest3 failed")
  sys.exit(1)
