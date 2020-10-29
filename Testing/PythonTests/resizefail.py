import os
import sys
from shapeworks import *

def resizefailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.resize()

  compareImg = Image(os.environ["DATA"] + "/resizefail.nrrd")

  return img.compare(compareImg)

val = resizefailTest()

if val is False:
  sys.exit(1)