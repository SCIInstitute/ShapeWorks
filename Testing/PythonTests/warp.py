import os
import sys
from shapeworks import *

def warpTest():
  img = Image(os.environ["DATA"] + "/input.nrrd")
  transform = ImageUtils.createWarpTransform(os.environ["DATA"] + "/source.particles", os.environ["DATA"] + "/target.particles", 1)
  img.applyTransform(transform)

  compareImg = Image(os.environ["DATA"] + "/warp1.nrrd")

  return img.compare(compareImg)

try:
  warpTest()
except ValueError:
  print("warpTest failed")
  sys.exit(1)

def warpfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  transform = ImageUtils.createWarpTransform(os.environ["DATA"] + "/source.particles")

  compareImg = Image(os.environ["DATA"] + "/warpfail.nrrd")

  return img.compare(compareImg)

try:
  warpfailTest()
  print("warpfailTest failed. The function requires target particles.")
  sys.exit(1)
except TypeError:
  sys.exit(0)
