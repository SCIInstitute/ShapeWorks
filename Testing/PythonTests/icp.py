import os
import sys
from shapeworks import *

def icpTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  imgSource = Image(os.environ["DATA"] + "/smooth1.nrrd")
  imgTarget = Image(os.environ["DATA"] + "/smooth2.nrrd")
  xform = imgSource.createTransform(imgTarget, TransformType.IterativeClosestPoint, 1.0, 5)
  img.applyTransform(xform, imgTarget.origin(), imgTarget.dims(), imgTarget.spacing(), imgTarget.coordsys(), InterpolationType.NearestNeighbor)

  compareImg = Image(os.environ["DATA"] + "/icp.nrrd")

  return img.compare(compareImg)

val = icpTest()

if val is False:
  sys.exit(1)