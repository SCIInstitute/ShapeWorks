import os
import sys
from shapeworks import *

def subfailTest():
  img1 = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img2 = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img = img1 - img2

  compareImg = Image(os.environ["DATA"] + "/subfail.nrrd")

  return img.compare(compareImg)

val = subfailTest()

if val is False:
  sys.exit(1)