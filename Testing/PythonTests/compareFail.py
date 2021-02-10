import os
import sys
import shapeworks as sw

def comparefailTest():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")

  compareImg = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")

  return img.compare(compareImg)

val = comparefailTest()

if val is False:
  sys.exit(1)