import os
import sys
from shapeworks import *

def warpTest():
  img = Image(os.environ["DATA"] + "/input.nrrd")
  transform = ImageUtils.createWarpTransform(os.environ["DATA"] + "/source.particles", os.environ["DATA"] + "/target.particles", 1)
  img.applyTransform(transform)

  compareImg = Image(os.environ["DATA"] + "/warp1.nrrd")

  return img.compare(compareImg)

val = warpTest()

if val is False:
  sys.exit(1)