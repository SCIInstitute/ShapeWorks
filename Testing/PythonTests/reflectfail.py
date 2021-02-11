import os
import sys
import shapeworks as sw

def reflectfailTest():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.reflect()

  compareImg = sw.Image(os.environ["DATA"] + "/reflectfail.nrrd")

  return img.compare(compareImg)

val = reflectfailTest()

if val is False:
  sys.exit(1)