import os
import sys
import shapeworks as sw

def icpTest():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  imgSource = sw.Image(os.environ["DATA"] + "/smooth1.nrrd")
  imgTarget = sw.Image(os.environ["DATA"] + "/smooth2.nrrd")
  xform = imgSource.createTransform(imgTarget, sw.TransformMethod.IterativeClosestPoint, 1.0, 5)
  img.applyTransform(xform, imgTarget.origin(), imgTarget.dims(), imgTarget.spacing(), imgTarget.coordsys(), sw.InterpolationType.NearestNeighbor)

  compareImg = sw.Image(os.environ["DATA"] + "/icp.nrrd")

  return img.compare(compareImg)

val = icpTest()

if val is False:
  sys.exit(1)