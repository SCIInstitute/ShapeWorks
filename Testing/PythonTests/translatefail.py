import os
import sys
from shapeworks import *

def translatefailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.translate()

  compareImg = Image(os.environ["DATA"] + "/translatefail.nrrd")

  return img.compare(compareImg)

val = translatefailTest()

if val is False:
  sys.exit(1)