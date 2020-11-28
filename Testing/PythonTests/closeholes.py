import os
import sys
from shapeworks import *

def closeholesTest1():
  img = Image(os.environ["DATA"] + "/image_with_holes.nrrd")
  img.closeHoles()

  compareImg = Image(os.environ["DATA"] + "/closedholes.nrrd")

  return img.compare(compareImg)

val = closeholesTest1()

if val is False:
  sys.exit(1)