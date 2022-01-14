import os
import sys
from shapeworks import *

success = True

def toImageTest1():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  img = mesh.toImage()

  compareImg = Image(os.environ["DATA"] + "/femurImage.nrrd")

  return img == compareImg

success &= utils.test(toImageTest1)

def toImageTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  bbox = mesh.boundingBox().pad(2.0) # changing spacing means we need to pad more to compensate
  img = mesh.toImage(bbox, spacing=[2.0, 2.0, 1.0])

  compareImg = Image(os.environ["DATA"] + "/femurImage2.nrrd")

  return img == compareImg

success &= utils.test(toImageTest2)

def toImageTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  img = mesh.toImage(region=mesh.boundingBox(), spacing=[2.29, 0.462, 1])

  compareImg = Image(os.environ["DATA"] + "/femurImage3.nrrd")

  return img == compareImg

success &= utils.test(toImageTest3)

sys.exit(not success)
