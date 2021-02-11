import os
import sys
import shapeworks as sw

def intensityTest():
  img = sw.Image(os.environ["DATA"] + "/nonBinary.nrrd")
  img.applyIntensityFilter(0, 100)

  compareImg = sw.Image(os.environ["DATA"] + "/intensity.nrrd")

  return img.compare(compareImg)

val = intensityTest()

if val is False:
  sys.exit(1)
