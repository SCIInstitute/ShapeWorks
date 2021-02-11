import os
import sys
import shapeworks as sw

def curvaturefailTest():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyCurvatureFilter(-5)

  compareImg = sw.Image(os.environ["DATA"] + "/curvaturefail.nrrd")

  return img.compare(compareImg)

val = curvaturefailTest()

if val is False:
  sys.exit(1)