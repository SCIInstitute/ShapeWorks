from pythonTestSetup import *

setup()

from shapeworks import *

def warpTest():
  img = Image(os.environ["DATA"] + "input.nrrd")
  transform = ImageUtils.createWarpTransform(os.environ["DATA"] + "source.particles", os.environ["DATA"] + "target.particles", 1)
  img.applyTransform(transform, InterpolationType.Linear)

  compareImg = Image(os.environ["DATA"] + "warp.nrrd")

  return img.compare(compareImg)

val = warpTest()

if val is False:
  sys.exit(1)