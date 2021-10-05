import os
import sys
from shapeworks import *

success = True

def negateTest():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img = -img

  compareImg = Image(os.environ["DATA"] + "/la-bin-negative.nrrd")

  return img.compare(compareImg)

success &= utils.test(negateTest)

sys.exit(not success)
