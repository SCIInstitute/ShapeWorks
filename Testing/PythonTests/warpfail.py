import os
import sys
from shapeworks import *

def warpfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  transform = ImageUtils.createWarpTransform(os.environ["DATA"] + "/source.particles")

  compareImg = Image(os.environ["DATA"] + "/warpfail.nrrd")

  return img.compare(compareImg)

val = warpfailTest()

if val is False:
  sys.exit(1)