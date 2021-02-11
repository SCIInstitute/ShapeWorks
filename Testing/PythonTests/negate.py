import os
import sys
import shapeworks as sw

def negateTest():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img = -img

  compareImg = sw.Image(os.environ["DATA"] + "/la-bin-negative.nrrd")

  return img.compare(compareImg)

val = negateTest()

if val is False:
  sys.exit(1)