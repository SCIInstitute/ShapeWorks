import os
import sys
import shapeworks as sw

def topofailTest():
  # <ctc> remove this test or figure out how to make it fail with scaling, sigmoidAlpha or sigmoidBeta
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  topoImg = img.topologyPreservingSmooth()

  compareImg = sw.Image(os.environ["DATA"] + "/topofail.nrrd")

  return topoImg.compare(compareImg)

val = topofailTest()

if val is False:
  sys.exit(1)
