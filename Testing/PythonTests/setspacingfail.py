import os
import sys
from shapeworks import *

def setspacingfailTest():
  img = Image(os.environ["DATA"] + "/la1-small.nrrd")
  img.setSpacing([0,0,0])

  compareImg = Image(os.environ["DATA"] + "/spacing1.nrrd")

  return img.compare(compareImg)

val = setspacingfailTest()

if val is False:
  sys.exit(1)
