import os
import sys
from shapeworks import *

def addfailTest():
  img1 = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img2 = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img = img1 + img2

  compareImg = Image(os.environ["DATA"] + "/addfail.nrrd")

  return img.compare(compareImg)

val = addfailTest()

if val is False:
  sys.exit(1)