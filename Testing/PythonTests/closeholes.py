import os
import sys
from shapeworks import *

def closeholesTest():
  img = Image(os.environ["DATA"] + "/image_with_holes.nrrd")
  img.closeHoles()

  compareImg = Image(os.environ["DATA"] + "/closedholes.nrrd")

  return img.compare(compareImg)

utils.test(closeholesTest)
