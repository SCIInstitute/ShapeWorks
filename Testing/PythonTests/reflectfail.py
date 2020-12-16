import os
import sys
from shapeworks import *

def reflectfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.reflect()

  compareImg = Image(os.environ["DATA"] + "/reflectfail.nrrd")

  return img.compare(compareImg)

val = reflectfailTest()

if val is False:
  sys.exit(1)