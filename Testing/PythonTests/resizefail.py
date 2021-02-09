import os
import sys
import shapeworks as sw

def resizefailTest():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resize()

  compareImg = sw.Image(os.environ["DATA"] + "/resizefail.nrrd")

  return img.compare(compareImg)

val = resizefailTest()

if val is False:
  sys.exit(1)