import os
import sys
from shapeworks import *

def tplevelsetfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")

  img.applyTPLevelSetFilter()

  compareImg = Image(os.environ["DATA"] + "/tplevelsetfail.nrrd")

  return img.compare(compareImg)

val = tplevelsetfailTest()

if val is False:
  sys.exit(1)