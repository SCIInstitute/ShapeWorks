import os
import sys
from shapeworks import *

success = True

def closeholesTest():
  img = Image(os.environ["DATA"] + "/image_with_holes.nrrd")
  img.closeHoles()

  compareImg = Image(os.environ["DATA"] + "/closedholes.nrrd")

  return img.compare(compareImg)

success &= utils.test(closeholesTest)

sys.exit(not success)
