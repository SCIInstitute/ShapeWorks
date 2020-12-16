import os
import sys
from shapeworks import *

def curvaturefailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyCurvatureFilter(-5)

  compareImg = Image(os.environ["DATA"] + "/curvaturefail.nrrd")

  return img.compare(compareImg)

val = curvaturefailTest()

if val is False:
  sys.exit(1)