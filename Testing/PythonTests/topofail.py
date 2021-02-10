import os
import sys
from shapeworks import *

def topofailTest():
    # <ctc> remove this test or figure out how to make it fail with scaling, sigmoidAlpha or sigmoidBeta
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = img.topologyPreservingSmooth()

  compareImg = Image(os.environ["DATA"] + "/topofail.nrrd")

  return topoImg.compare(compareImg)

val = topofailTest()

if val is False:
  sys.exit(1)
