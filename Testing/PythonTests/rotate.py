import os
import sys
from shapeworks import *

success = True

def rotateTest1():
  img = Image(os.environ["DATA"] + "/la-bin-centered.nrrd")
  img.rotate(0.7854, [1.0, 1.0, 1.0])

  compareImg = Image(os.environ["DATA"] + "/rotate2.nrrd")

  return img.compare(compareImg)

success &= utils.test(rotateTest1)

def rotateTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  mesh.rotate(90, Axis.Y)

  compareMesh = Mesh(os.environ["DATA"] + "/rotated_mesh.ply")

  return mesh == compareMesh
  
success &= utils.test(rotateTest2)


sys.exit(not success)
