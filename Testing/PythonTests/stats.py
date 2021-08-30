import os
import sys
from shapeworks import *

def statsTest():
  image = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  imgMin = image.min()
  imgMax = image.max()
  imgMean = image.mean()
  imgStd = image.std()

  arr = image.toArray()
  arrMin = arr.min()
  arrMax = arr.max()
  arrMean = arr.mean()
  arrStd = arr.std() 

  return abs(imgMin - arrMin) < 1e-4 and abs(imgMax - arrMax) < 1e-4 and abs(imgMean - arrMean) < 1e-4 and abs(imgStd - arrStd) < 1e-4

utils.test(statsTest)
