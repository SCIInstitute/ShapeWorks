import os
import sys
import shapeworks as sw

def divTest1():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img /= 3.14

  compareImg = sw.Image(os.environ["DATA"] + "/baseline_divide.nrrd")

  return img.compare(compareImg)

val = divTest1()

if val is False:
  sys.exit(1)