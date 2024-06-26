import os
import sys
from shapeworks import *

success = True

def binarizeTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.binarize()

  compareImg = Image(os.environ["DATA"] + "/binarize1.nrrd")

  return img.compare(compareImg)

success &= utils.test(binarizeTest1)

def binarizeTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.binarize(minVal=-0.1)

  compareImg = Image(os.environ["DATA"] + "/binarize2.nrrd")

  return img.compare(compareImg)

success &= utils.test(binarizeTest2)

sys.exit(not success)
