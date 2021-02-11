import os
import sys
import shapeworks as sw

def antialiasfailTest():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.antialias(iterations=1)

  compareImg = sw.Image(os.environ["DATA"] + "/antialias3.nrrd")

  return img.compare(compareImg)

val = antialiasfailTest()

if val is False:
  sys.exit(1)