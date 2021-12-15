import os
import sys
from shapeworks import *

success = True

def isolateTest():
  img = Image(os.environ["DATA"] + "/isolate_input.nrrd")
  img.isolate()

  compareImg = Image(os.environ["DATA"] + "/isolate_output.nrrd")

  return img.compare(compareImg)

success &= utils.test(isolateTest)

sys.exit(not success)
