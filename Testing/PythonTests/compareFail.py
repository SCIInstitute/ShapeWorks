import os
import sys
from shapeworks import *

def comparefailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")

  compareImg = Image(os.environ["DATA"] + "/la-bin.nrrd")

  return img.compare(compareImg)

val = comparefailTest()

if val is False:
  sys.exit(1)