import os
import sys
import shapeworks as sw

def warpfailTest():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  transform = sw.ImageUtils.createWarpTransform(os.environ["DATA"] + "/source.particles")

  compareImg = sw.Image(os.environ["DATA"] + "/warpfail.nrrd")

  return img.compare(compareImg)

val = warpfailTest()

if val is False:
  sys.exit(1)