import os
import sys
import shapeworks as sw

def tplevelsetfailTest():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyTPLevelSetFilter()

  compareImg = sw.Image(os.environ["DATA"] + "/tplevelsetfail.nrrd")

  return img.compare(compareImg)

val = tplevelsetfailTest()

if val is False:
  sys.exit(1)