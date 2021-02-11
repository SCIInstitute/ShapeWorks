import os
import sys
import shapeworks as sw

def gradientTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyGradientFilter()

  compareImg = sw.Image(os.environ["DATA"] + "/gradient.nrrd")

  return img.compare(compareImg)

val = gradientTest1()

if val is False:
  sys.exit(1)