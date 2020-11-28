import os
import sys
from shapeworks import *

def antialiasfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.antialias(iterations=1)

  compareImg = Image(os.environ["DATA"] + "/antialias3.nrrd")

  return img.compare(compareImg)

val = antialiasfailTest()

if val is False:
  sys.exit(1)