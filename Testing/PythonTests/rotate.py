import os
import sys
import shapeworks as sw

def rotateTest1():
  img = sw.Image(os.environ["DATA"] + "/la-bin-centered.nrrd")
  img.rotate(0.7854, [1.0, 1.0, 1.0])

  compareImg = sw.Image(os.environ["DATA"] + "/rotate2.nrrd")

  return img.compare(compareImg)

val = rotateTest1()

if val is False:
  sys.exit(1)