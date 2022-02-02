import os
import sys
from shapeworks import *

success = True

def infoTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  size1 = img.size()

  compareImg = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  size2 = compareImg.size()

  return size1[0] == size2[0] and size1[1] == size2[1] and size1[2] == size2[2]

success &= utils.test(infoTest1)

def infoTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  dims1 = img.dims()

  compareImg = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  dims2 = compareImg.dims()

  return dims1[0] == dims2[0] and dims1[1] == dims2[1] and dims1[2] == dims2[2]

success &= utils.test(infoTest2)

sys.exit(not success)
