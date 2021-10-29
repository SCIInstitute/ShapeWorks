import os
import sys
from shapeworks import *

success = True

def recenterTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.recenter()

  compareImg = Image(os.environ["DATA"] + "/recenter.nrrd")

  return img.compare(compareImg)

success &= utils.test(recenterTest)

sys.exit(not success)
