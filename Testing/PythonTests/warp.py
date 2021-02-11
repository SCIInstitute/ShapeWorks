import os
import sys
import shapeworks as sw

def warpTest():
  img = sw.Image(os.environ["DATA"] + "/input.nrrd")
  transform = sw.ImageUtils.createWarpTransform(os.environ["DATA"] + "/source.particles", os.environ["DATA"] + "/target.particles", 1)
  img.applyTransform(transform)

  compareImg = sw.Image(os.environ["DATA"] + "/warp1.nrrd")

  return img.compare(compareImg)

val = warpTest()

if val is False:
  sys.exit(1)