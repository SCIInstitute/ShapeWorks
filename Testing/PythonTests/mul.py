import os
import sys
import shapeworks as sw

def mulTest1():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img *= 3.14

  compareImg = sw.Image(os.environ["DATA"] + "/baseline_multiply.nrrd")

  return img.compare(compareImg)

val = mulTest1()

if val is False:
  sys.exit(1)