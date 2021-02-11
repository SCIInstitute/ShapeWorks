import os
import sys
import shapeworks as sw

def translatefailTest():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.translate()

  compareImg = sw.Image(os.environ["DATA"] + "/translatefail.nrrd")

  return img.compare(compareImg)

val = translatefailTest()

if val is False:
  sys.exit(1)