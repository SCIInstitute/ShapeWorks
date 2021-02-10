import os
import sys
import shapeworks as sw

def icpTest():
  imgSource = sw.Image(os.environ["DATA"] + "/smooth1.nrrd")
  imgTarget = sw.Image(os.environ["DATA"] + "/smooth2.nrrd")
  xform = imgSource.createTransform(imgTarget, sw.TransformType.IterativeClosestPoint, 1.0, 5)
  imgSource.applyTransform(xform, imgTarget.origin(), imgTarget.dims(), imgTarget.spacing(), imgTarget.coordsys(), sw.Image.InterpolationType.NearestNeighbor)

  compareImg = sw.Image(os.environ["DATA"] + "/icp.nrrd")

  return imgSource == compareImg

val = icpTest()

if val is False:
  sys.exit(1)
