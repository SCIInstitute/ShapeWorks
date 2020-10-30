import os
import sys
from shapeworks import *

def topofailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = ImageUtils.topologyPreservingSmooth()

  compareImg = Image(os.environ["DATA"] + "/topofail.nrrd")

  return topoImg.compare(compareImg)

val = topofailTest()

if val is False:
  sys.exit(1)